import socket
import agent
import json

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
            self.update()

    def getAgent(self, key):
        if key == -1:
            return agent.Agent(100)
        
        if not key in self.agents:
            self.agents[key] = agent.Agent(1500)

        return self.agents[key]        

    @staticmethod
    def getData(sock):
        size = int.from_bytes(sock.recv(4), "little", signed=True)
        data = []
        for i in range(size):
            data.append(int.from_bytes(sock.recv(4), "little", signed=True))
        return data

    def update(self):
        sock = self.serverSock.accept()[0]
        data = Server.getData(sock)

        agent1 = self.getAgent(data[0]) 
        currentElo = agent1.elo
        deltaElo = 0

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
            with open("elos.json", "r") as file:
                tmp = json.load(file)
            print(tmp)

            for key, elo in tmp.items():
                self.agents[int(key)] = agent.Agent(elo)
        except Exception as e:
            print(e)

    def save(self):
        d = {}
        for key, agent in self.agents.items():
            d[key] = agent.elo
            
        with open("elos.json", "w") as file:
            json.dump(d, file)
        
if __name__ == "__main__":
    server = Server()
