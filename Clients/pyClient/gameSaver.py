import socket
import pickle

def send(data):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(("wandhoven.ddns.net", 2551))
    sock.send((-2).to_bytes(4, "little", signed=True))

    b = pickle.dumps(data)
    sock.send(len(b).to_bytes(4, "little", signed=True))
    sock.send(b)

    size = int.from_bytes(sock.recv(4), "little", signed=True)
    data = pickle.loads(sock.recv(size))
    return data

#send(("connect4", ["game_counter", "tie"], 0))
#send(("connect4", ["game_counter", "lose"],0))

def sendFull(data, win):
    if win == 1:
        typ = "win"
        
    elif win == -1:
        typ = "lose"
        
    elif win == 0:
        typ = "tie"
        
    identity = send(("connect4", ["game_counter", typ]))
    send(("connect4", ["games", typ, str(hex(identity))], data))
    send(("connect4", ["game_counter", typ], identity+1))

    print(identity)
