import os
from json import dump as json_dump
from config import PATH, GAMES

def createFileIfNecesary(filename, content):
  """create a file in data if necesary"""
  os.mkdir(os.path.join(PATH, "data"))
  filename = os.path.join("data", filename)
  _createFileIfNecesary(filename, content)

def _createFileIfNecesary(filename, content): 
  """create a file in PATH if necesary"""
  filename = os.path.join(PATH, filename)
  if os.path.exists(filename):
    print('file already exists')
  else:
    # create a file
    with open(filename, 'w') as fp:
        # uncomment if you want empty file
        json_dump(content, fp)

def setupAccounts():
  """setup accounts file"""
  createFileIfNecesary("accounts.json", { "accountId": -10 })

def setupGameLogs(game):
  """setup game data file"""
  createFileIfNecesary(game + ".json", {
      "game_counter": {
        "lose": 0,
        "tie": 0,
        "win": 0
      }
    })

def setup():
  """setup necesary files"""
  _createFileIfNecesary("elos.json", {})

  setupAccounts()
  for game in GAMES:
    setupGameLogs(game)

