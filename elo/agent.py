import score

def updateScore(Ra, K, Sa, Ea):
    return Ra + K*(Sa - Ea)

class Agent:
    def __init__(self, elo):
        self.elo = elo
        self.expectedScore = score.PredictedScores()
        self.realScore = score.Score()

    def addGamePrediction(self, other):
        self.expectedScore.addGame(self, other)

    def update(self, k):
        self.elo = updateScore(self.elo, k, self.realScore.score, self.expectedScore.score)
        self.realScore.score = 0
        self.expectedScore.score = 0

    

def addGame(agent1, agent2, win):
    if win == 1:
        agent1.realScore.addWin()
    elif win == 0:
        agent1.realScore.addTie()
    else:
        agent1.realScore.addLoss()
    

def getElo(agent1, agent2):
    return 1/(1+10**((agent1.elo - agent2.elo)/400))

def getPredictedElo(agent1, agent2):
    elo = {}
    elo[agent1] = getElo(agent1, agent2)
    elo[agent2] = getElo(agent2, agent1)
    return elo

def update(agent1, score1, agent2, score2, k):
    agent1.elo = updateScore(agent1.elo, k, getElo(agent1, agent2), score1)
    agent2.elo = updateScore(agent2.elo, k, getElo(agent2, agent1), score2)
