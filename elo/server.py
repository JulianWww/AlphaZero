import socket
import agent
import json
import pickle
from os.path import join as joinPath

PATH = "/media/A/MyCode/AlphaZero/elo"
print(PATH)

class Server:
    def __init__(self):
        self.load()
        self.serverSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.serverSock.bind(("", 2551))
        self.serverSock.listen(5)
        self.main()

    def main(self):
        while True:
            print("waiting for connection")
            sock = self.serverSock.accept()[0]
            data = Server.getData(sock)
            if (data[0] == 1):
                self.update_elo(data[1], sock)
            elif (data[0] == 2):
                out = pickle.dumps(self.update_data(data[1]))

                sock.send(len(out).to_bytes(4, "little", signed=True))
                sock.send(out)

    def update_data(self, data):
        try:
            with open(joinPath(PATH, "data", f"{data[0]}.json"), "r") as file:
                info = json.load(file)
        except:
            info = {}

        sub = info
        for key in data[1][:-1]:
            try:
                sub = sub[key]
            except KeyError:
                sub[key] = {}    
                sub = sub[key]

        if len(data) == 3:
            sub[data[1][-1]] = data[2]
            
            with open(joinPath(PATH, "data", f"{data[0]}.json"), "w") as file:
                json.dump(info, file, sort_keys=True, indent=2)
            return True
        else:
            try:
                return sub[data[1][-1]]
            except KeyError:
                return None

    def getAgent(self, key):
        if key == -1:
            return agent.Agent(100)
        
        if not key in self.agents:
            self.agents[key] = agent.Agent(100)

        return self.agents[key]        

    @staticmethod
    def getData(sock):
        size = int.from_bytes(sock.recv(4), "little", signed=True)
        data = []
        if size == -1:
            data.append(-1)
            data.append(int.from_bytes(sock.recv(4), "little", signed=True))
            return (1, data)

        if size == -2:
            size = int.from_bytes(sock.recv(4), "little", signed=True)
            data = pickle.loads(sock.recv(size))
            return (2, data)
            
        for i in range(size):
            data.append(int.from_bytes(sock.recv(4), "little", signed=True))
        return (1, data)

    def update_elo(self, data, sock):
        deltaElo = 0

        if data[0] == -1:
            closest = None
            idx = list(self.agents.keys())[0]
            for _idx, agent in self.agents.items():
                if (_idx > 0):
                    if closest is None:
                        if data[1] < agent.elo:
                            print(agent.elo)
                            idx = _idx
                            closest = agent
                        
                    elif (abs(data[1] - closest.elo) > abs(data[1] - agent.elo) and data[1] < agent.elo):
                        idx = _idx
                        closest = agent
            deltaElo = idx
            
        else:
            agent1 = self.getAgent(data[0]) 
            currentElo = agent1.elo
            
            if len(data) == 3:
                agent2 = self.getAgent(data[1])
                agent1.addGamePrediction(agent2)
                agent.addGame(agent1, agent2, data[2])
                agent1.update(32)

                deltaElo = abs(agent1.elo - currentElo)
                
            elif len(data) == 2:
                agent1.elo = data[1]
                deltaElo = abs(agent1.elo - currentElo)

            elif len(data) == 1:
                deltaElo = agent1.elo

        sock.send(int(deltaElo).to_bytes(4, "little", signed=True))
        sock.close()
        self.save()
    
    def load(self):
        self.agents = {}
        try:
            with open(joinPath(PATH, "elos.json"), "r") as file:
                tmp = json.load(file)

            for key, elo in tmp.items():
                self.agents[int(key)] = agent.Agent(elo)
        except Exception as e:
            print(e)

    def save(self):
        d = {}
        for key, agent in self.agents.items():
            d[key] = agent.elo
            
        with open(joinPath(PATH, "elos.json"), "w") as file:
            json.dump(d, file, sort_keys=True, indent=2)
        
if __name__ == "__main__":
    while True:
        try:
            server = Server()
        except: pass
