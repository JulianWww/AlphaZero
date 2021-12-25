import socket
import gameSaver
import math

class DummyAgent(gameSaver.DummyClient):
    def render(*args):
        "dummy function to avoid some logic in the caller"
        pass

    def winScreen(*args):
        "see render"
        pass

    def updateElo(*args):
        "dummy function"
        pass

class RemoteClient(DummyAgent):
    """
    Remo Client connect to server sends state and receves recomended action
    """
    def __init__(self, ip, port):
        self.ip = ip
        self.port = port
        self.setVersion()

    def setVersion(self):
        account = gameSaver.getAccount()
        self.version = gameSaver.getClientWithClosestElo(account)
        print("you are playing aginst version:", self.version)
           
    def connect(self):
        """Astablisch connection to the Server"""
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((self.ip, self.port))
        return client_socket

    @staticmethod
    def getData(sock):
        "actually get action from server"
        data = sock.recv(8*4)
        return int.from_bytes(data, "little", signed=True)

    def sendState(self, sock, state):
        "send the satate to the server"
        binaryState = self.stateToBinary(state)
        sock.send(binaryState)

    def getAction(self, state):
        "get action from server"
        sock = self.connect()
        self.sendState(sock, state.toServerProtocol())
        return RemoteClient.getData(sock)

    def stateToBinary(self, state):
        "convert state to binary array to be sent to server"
        out = bytearray()
        for val in state:
            out += RemoteClient.intToBinArr(val)
        out += RemoteClient.intToBinArr(self.version)
        return out

    def updateElo(self, win):
        otherElo = gameSaver.getElo(self.version)
        myElo = gameSaver.getMyElo()
        print(myElo, otherElo)
        expected = 1/(1+math.e**((otherElo - myElo)/400))

        newElo = myElo + 256 * (win - expected)
        gameSaver.setMyElo(newElo)
        self.setVersion()
        print("your new elo is: ", newElo)

class GameReplayAgent(DummyAgent):
    def __init__(self, end, key, file):
        self.actions = gameSaver.send(
            (file, ["games", end, str(hex(key))])
        )
        self.iterator = 0

    def getAction(self, state):
        action = self.actions[self.iterator]
        self.iterator += 1
        return action
