from game import Game
from Client import RemoteClient
from GUI import GUI, ConsoleAgent
from threading import Thread
from random import seed
from time import time, sleep
import pickle


def render(agents, game):
    "render the state for all agents"
    for agent in agents.values():
            agent.render(game)

def endScreens(agents, game):
    "render end screen for all agents"
    for player, agent in agents.items():
            agent.winScreen(game, -player*game.player*game.isDone)

def run(game, agents, gui):
    "call the agents, render and get action to play a game"
    while True:
        game.reset()
        while not game.isDone:
            render(agents, game)
            action = agents[game.player].getAction(game)
            game.takeAction(action)

        endScreens(agents, game)
        render(agents, game)
        sleep(5)

    
def getAgents(agent1, agent2, game):
    """get dict mapping player actions id's to agents"""
    val = game.starter*2-1
    out = {
        +val: agent1,
        -val: agent2
    }
    return out

if __name__ == "__main__":
    seed(time())
    while True:
        client = RemoteClient("localhost", Game.port)
        game = Game()
        gui = GUI()
        agents = getAgents(gui, client, game)
        runner = Thread(target=run, args=(game, agents, gui))
        runner.start()
        gui.mainloop()
