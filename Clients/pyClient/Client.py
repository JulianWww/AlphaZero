import socket
import gameSaver

class DummyAgent():
    def render(*args):
        "dummy function to avoid some logic in the caller"
        pass

    def winScreen(*args):
        "see render"
        pass

class RemoteClient(DummyAgent):
    """
    Remo Client connect to server sends state and receves recomended action
    """
    def __init__(self, ip, port):
        self.ip = ip
        self.port = port
           
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

    @staticmethod
    def sendState(sock, state):
        "send the satate to the server"
        binaryState = RemoteClient.stateToBinary(state)
        sock.send(binaryState)

    def getAction(self, state):
        "get action from server"
        sock = self.connect()
        RemoteClient.sendState(sock, state.toServerProtocol())
        return RemoteClient.getData(sock)

    @staticmethod
    def intToBinArr(my_int):
        "converts a number to 4 byte binary to send to server"
        out = bytearray()
        for e in [my_int >> i & 0xff for i in (0,8,16,24)]:
            out.append(e)
        return out

    @staticmethod
    def stateToBinary(state):
        "convert state to binary array to be sent to server"
        out = bytearray()
        for val in state:
            out += RemoteClient.intToBinArr(val)
        return out

class GameReplayAgent(DummyAgent):
    def __init__(self, end, key):
        self.actions = gameSaver.send(
            ("connect4", ["games", end, str(hex(key))])
        )
        self.iterator = 0

    def getAction(self, state):
        action = self.actions[self.iterator]
        self.iterator += 1
        return action
