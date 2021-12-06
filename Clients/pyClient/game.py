import json, pickle
from random import getrandbits
from tkinter import simpledialog

#get all win postions
with open("winStates.json", "r") as file:
    winStates = json.load(file)

def getLoad():
    inp = input("there is a game available, do you want to load it? [y/n]: ")
    if inp == "y":
        return True
    elif inp == "n":
        return False
    return getLoad()

class Game:
    "Class containing game rules"
    name = "connect4"
    port = 12345

    pieces = {
        1:  "X",
        0:  "-",
        -1: "O"
    }

    def __init__(self):
        self.reset()

    def reset(self):
        "reset game to default"
        self.tie = False

        self.board = [0 for i in range(42)]
        self.player = 1

        self.isDone = self.getIsDone()
            
        self.ends = []
        self.getAllowedActions()

    def actionModifier(self, action):
        "for console client convert the inputed action to the internal game action"
        for potAction in self.allowedActions:
            if potAction % 7 == action:
                return potAction
        return -1

    @staticmethod
    def encodeAction(x, y):
        "convert position to action"
        return x + 7*y

    @staticmethod
    def decodeAction(action):
        return action%7, action//7

    def getAllowedActions(self):
        "get the allowd actions and write to allowedActions list"
        self.allowedActions = []
        for x in range(7):
            if self.board[self.encodeAction(x, 0)] == 0:
                hasPassed = False
                for y in range(5):
                    if self.board[self.encodeAction(x, y)] == 0 and self.board[self.encodeAction(x, y+1)] != 0:
                        self.allowedActions.append(self.encodeAction(x, y))
                        hasPassed = True
                        break
                if not hasPassed:
                    self.allowedActions.append(self.encodeAction(x, 5))

    def takeAction(self, action):
        "if action is valid (in allowedActions) modify game to perform move"
        if action in self.allowedActions:
            self.board[action] = self.player
            self.isDone = self.getIsDone()
            self.player = -self.player
            self.getAllowedActions()
                    
        
    def consoleRender(self):
        "render state to Console"
        for y in range(6):
            for x in range(7):
                if self.encodeAction(x, y) in self.allowedActions:
                    print("+ ", end="")
                else:
                    print(self.pieces[self.board[x+y*7]],end=" ")

            print("")
        print("")
        print(0,1,2,3,4,5,6)
        print(f"player {self.pieces[self.player]} is up")

    def toServerProtocol(self):
        "convert to binary int array to send to server"
        out = [0] * 85
        out[-1] = self.player
        for idx, val in enumerate(self.board):
            if val == 1:
                out[idx] = 1
            elif val == -1:
                out[idx + len(self.board)] = 1
        return out

    def getIsDone(self):
        "check if game is done"
        if self.board.count(0) == 0:
            self.tie = True
            return True
        
        done = False
        for option in winStates:
            val = 0
            for pos in option:
                val += self.board[pos]

            if val == 4*self.player:
                done = True
                self.ends.append((option[0], option[-1]))
                
        return done

    
