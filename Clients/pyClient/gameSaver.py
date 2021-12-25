import socket
import pickle

class DummyClient():
    @staticmethod
    def intToBinArr(my_int):
        "converts a number to 4 byte binary to send to server"
        out = bytearray()
        for e in [my_int >> i & 0xff for i in (0,8,16,24)]:
            out.append(e)
        return out

def connect():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(("wandhoven.ddns.net", 2551))
    return sock

def send(data):
    sock = connect()
    
    sock.send((-2).to_bytes(4, "little", signed=True))

    b = pickle.dumps(data)
    sock.send(len(b).to_bytes(4, "little", signed=True))
    sock.send(b)

    size = int.from_bytes(sock.recv(4), "little", signed=True)
    data = pickle.loads(sock.recv(size))
    return data

def getClientWithClosestElo(account):
    myElo = send(("accounts", ["elos", str(account)]))
    sock = connect()

    sock.send(int(-1).to_bytes(4, "little", signed=True))
    sock.send(int(myElo).to_bytes(4, "little", signed=True))

    other = int.from_bytes(sock.recv(4), "little", signed=True)
    return other

def getMyElo():
    account = getAccount()
    myElo = send(("accounts", ["elos", str(account)]))
    if myElo < 100:
        myElo = 100
    return myElo

def setMyElo(elo):
    account = getAccount()
    myElo = send(("accounts", ["elos", str(account)], elo))
    if myElo < 100:
        myElo = 100
    return myElo

def getElo(account):
    sock = connect()

    sock.send(int(1).to_bytes(4, "little", signed=True))
    sock.send(int(account).to_bytes(4, "little", signed=True))

    other = int.from_bytes(sock.recv(4), "little", signed=True)
    return other


def reset():
    send(("accounts", ["accountId"], -10))
    send(("connect4", ["game_counter", "tie"], 0))
    send(("connect4", ["game_counter", "lose"],0))
    send(("connect4", ["game_counter", "win"], 0))

def sendFull(data, win):
    if win == 1:
        typ = "win"
        
    elif win == -1:
        typ = "lose"
        
    elif win == 0:
        typ = "tie"
        
    identity = send(("connect4", ["game_counter", typ]))
    send(("connect4", ["games", typ, str(hex(identity))], data))
    print("id: ", (typ, identity))
    send(("connect4", ["game_counter", typ], identity+1))


def getAccount():
    try:
        with open("account.p", "rb") as file:
            return pickle.load(file)
    except:
        print("creating account file")
        account = send(("accounts", ["accountId"]))
        send(("accounts", ["accountId"], account - 1))
        send(("accounts", ["elos", str(account)], 100))
        with open("account.p", "wb") as file:
            pickle.dump(account, file)
        return account
