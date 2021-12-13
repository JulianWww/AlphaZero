import socket
import pickle

def send(data):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(("127.0.0.1", 2551))
    sock.send((-2).to_bytes(4, "little", signed=True))

    b = pickle.dumps(data)
    sock.send(len(b).to_bytes(4, "little", signed=True))
    sock.send(b)

    size = int.from_bytes(sock.recv(4), "little", signed=True)
    data = pickle.loads(sock.recv(size))
    print(data)

send(("ju", ["test", "var1"], "hi"))
