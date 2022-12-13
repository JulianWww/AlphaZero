def getElo(agent1, agent2):
    return 1/(1+10**((agent2.elo - agent1.elo)/400))

class Score:
    """simple score counter"""
    def __init__(self):
        self.score = 0

    def addWin(self):
        self.score += 1

    def addTie(self):
        self.score += 0.5

    def addLoss(self):
        pass

class PredictedScores:
    """score prediction using elo forward function"""
    def __init__(self):
        self.score = 0

    def addGame(self, this, other):
        self.score += getElo(this, other)
