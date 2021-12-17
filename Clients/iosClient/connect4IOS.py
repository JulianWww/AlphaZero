import scene, socket, requests, pickle
from random import getrandbits
from copy import copy
import threading, time
from select import select as sockSelect

winStates = [
 [0,1,2,3],[1,2,3,4],[2,3,4,5],[3,4,5,6],[7,8,9,10],[8,9,10,11],[9,10,11,12],[10,11,12,13],[14,15,16,17],[15,16,17,18],[16,17,18,19],[17,18,19,20],[21,22,23,24],[22,23,24,25],[23,24,25,26],[24,25,26,27],[28,29,30,31],[29,30,31,32],[30,31,32,33],[31,32,33,34],[35,36,37,38],[36,37,38,39],[37,38,39,40],[38,39,40,41],[0 , 7,14,21],[7 ,14,21,28],[14,21,28,35],[1 ,8 ,15,22],[8 ,15,22,29],[15,22,29,36],[2,9,16,23],[9,16,23,30],[16,23,30,37],[3 ,10,17,24],[10,17,24,31],[17,24,31,38],[4 ,11,18,25],[11,18,25,32],[18,25,32,39],[5 ,12,19,26],[12,19,26,33],[19,26,33,40],[6 ,13,20,27],[13,20,27,34],[20,27,34,41],[14,22,30,38],[7, 15,23,31],[15,23,31,39],[0, 8, 16,24],[8, 16,24,32],[16,24,32,40],[1, 9 ,17,25],[9, 17,25,33],[17,25,33,41],[2, 10,18,26],[10,18,26,34],[3, 11,19,27],[3, 9, 15,21],[4, 10,16,22],[10,16,22,28],[5, 11,17,23],[11,17,23,29],[17,23,29,35],[6, 12,18,24],[12,18,24,30],[18,24,30,36],[13,19,25,31],[19,25,31,37],[20,26,32,38]
]

responce = requests.get("http://wandhoven.ddns.net/code/AlphaZero/connect4ServerIP.txt")
ip = responce.content
port = 25500


def send(data):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(("wandhoven.ddns.net", 2551))
    sock.send((-2).to_bytes(4, "little", signed=True))

    b = pickle.dumps(data)
    sock.send(len(b).to_bytes(4, "little", signed=True))
    sock.send(b)

    size = int.from_bytes(sock.recv(4), "little", signed=True)
    data = pickle.loads(sock.recv(size))
    return data

def reset():
    send(("connect4", ["game_counter", "tie"], 0))
    send(("connect4", ["game_counter", "lose"],0))
    send(("connect4", ["game_counter", "win"], 0))

def sendFull(data, win):
    if win == 1:
        typ = "win"
        
    elif win == -1:
        typ = "lose"
        
    elif win == 0:
        typ = "tie"
        
    identity = send(("connect4", ["game_counter", typ]))
    send(("connect4", ["games", typ, str(hex(identity))], data))
    print("id: ", (typ, identity))
    send(("connect4", ["game_counter", typ], identity+1))

class gameLog:
        def __init__(self):
                self.actions = []

        def send(self, win):
                if len(self.actions):
                        print(win, self.actions)
                        sendFull(self.actions, win)

class game:
	def __init__(self):
		self._reset()
		self.log = gameLog()

	def reset(self):
		self.log.send(self.win)
		self._reset()
				
	def _reset(self):
		self.board = [0 for i in range(42)]
		self.player = 1
		self.allowedActions = self.getAllowedActions()
		self.isDone = self.getIsDone()
		self.ends = []
		self.tie = False
		self.win = 0
				
	@staticmethod
	def encodeAction(x, y):
		return x + 7*y
			
	@staticmethod
	def decodeAction(action):
		return action%7, action//7
			
	def getAllowedActions(self):
		allowed = []
		for x in range(7):
			if (self.board[game.encodeAction(x, 0)] == 0):
				hasPassed = False
				for y in range(5):
					if (self.board[game.encodeAction(x, y)] == 0 and self.board[game.encodeAction(x, y+1)] != 0):
						allowed.append(self.encodeAction(x,y))
						hasPassed = True
						break
				if (not hasPassed):
					allowed.append(self.encodeAction(x, 5))
		return allowed
		
	def takeAction(self, action):
		if (action in self.allowedActions):
			self.board[action] = self.player
			self.isDone = self.getIsDone()
			self.player = -self.player
			self.allowedActions = self.getAllowedActions()
			self.log.actions.append(action)
		
	def toServerProtocol(self):
		out = [0]*85
		out[-1] = self.player
		for idx, val in enumerate(self.board):
			if (val == 1):
				out[idx] = 1
			if (val == -1):
				out[idx + len(self.board)] = 1
		return out
		
	def getIsDone(self):
		if (self.board.count(0) == 0):
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
								
	def render(self):
		for idx, val in enumerate(self.board):
			if val == 0:
				print("-", end=" ")
			if val == 1:
				print("X", end=" ")
			if val ==-1:
				print("O", end=" ")
			if idx%7 == 6:
				print("")

class Client:
	def __init__(self, ip, port):
		self.ip = ip
		self.port = port
			
	def connect(self):
		client_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		client_sock.connect((self.ip, self.port))
		return client_sock
		
	@staticmethod
	def getData(sock, gui):
		data = sock.recv(8*4)
		return int.from_bytes(data, "little", signed=True)
		
	@staticmethod
	def sendState(sock, state):
		binaryState = Client.stateToBinary(state)
		sock.send(binaryState)
		
	def getAction(self, state, gui):
		sock = self.connect()
		self.sendState(sock, state.toServerProtocol())
		return self.getData(sock, gui)
				
	@staticmethod
	def intToBinArr(my_int):
		out = bytearray()
		for e in [my_int >> i & 255 for i in (0,8,16,24)]:
			out.append(e)
		return out
		
	@staticmethod
	def stateToBinary(state):
		out = bytearray()
		for val  in state:
			out += Client.intToBinArr(val)
		return out
ai = Client(ip, port)

class Connect4GUI(scene.Scene):
	def setup(self):
		self.board = []
		self.sprites = []
		self.label = None
				
		self.GUIPlayer = 0
		self.background_color = "black"
		self.game = game()
		self.renderBoard()
		self.reset()
				
		self.aiThread = threading.Thread(target=self.serverUpdate)
		self.aiThread.start()
		
	def reset(self):
		self.GUIPlayer = getrandbits(1)*2-1
		self.game.reset()
		self.lastState = copy(self.game.board)
		for n in self.sprites:
			n.remove_from_parent()
		self.custom_update()
				
		if not self.label is None:
			self.label.remove_from_parent()
				
		
	def serverUpdate(self):
		while True:
			if (self.GUIPlayer != self.game.player and not self.game.isDone):
				action = ai.getAction(self.game, self)
				self.game.takeAction(action)
				self.custom_update()
			
	def custom_update(self):
		if (self.GUIPlayer == self.game.player):
			self.background_color = "green"
		else:
			self.background_color = "black"
		self.renderPieces(self.game.board)
		self.renderAllowedActions(self.game.allowedActions)
		if (self.game.isDone):
			self.game.win = -1 if (self.GUIPlayer != self.game.player) else 1
			self.game.win = 0 if (self.game.tie) else self.game.win
                        
			txt = "you Win" if (self.game.win == -1) else "you Loose"
			txt = "Tie" if (self.game.win == 0) else txt
			self.label = scene.LabelNode(txt)
			self.label.position = self.size[0]/2, self.getSize() * 7
			self.add_child(self.label)
						
	def _getPos(self):
		size = self.getSize()
		return size, size
				
	def touchToPos(self, touch):
		x, y = touch.location
		xSize, ySize = self._getPos()
		return int((x - xSize * 0.5) // xSize), int(5-(y - ySize * 0.5) // ySize)
				
	def touch_ended(self, touch):
		if (self.game.isDone):
			self.reset()
			return;
						
		elif (self.GUIPlayer == self.game.player):
			x, y = self.touchToPos(touch)
			action = self.game.encodeAction(x, y)
			self.game.takeAction(action)
			self.custom_update()
				
	def getPiecePos(self, x, y):
		xPos = (x+1) * self._getPos()[0]
		yPos = (6-y) * self._getPos()[1]
		return xPos, yPos
			
	def getSize(self):
		height = self.size[0]/8
		weight = self.size[1]/8
		return min(height, weight)
		
	def renderBoard(self):
		for y in range(6):
			for x in range(7):
				xPos, yPos = self.getPiecePos(x, y)
				size = self.getSize()
				sprite = scene.SpriteNode('plf:Tile_BoxCoin_disabled_boxed')
				sprite.position = (xPos, yPos)
				sprite.size = (self.getSize(), self.getSize())
				self.add_child(sprite)
				self.run_action(scene.Action.wait(2))
				self.board.append(sprite)
		
	def renderAllowedActions(self, allowedActions):
		return
		size = self.getSize()
		for idx, sprite in enumerate(self.board):
			sprite.texture = scene.Texture('plf:Tile_BoxCoin_disabled_boxed')
			if idx in allowedActions and self.GUIPlayer == self.game.player:
				sprite.texture = scene.Texture('plf:Tile_BoxCoin_boxed')
						
			sprite.size = (size, size)
		
	def renderPieces(self, state):
		for x in range(7):
			for y in range(6):
				if (state[game.encodeAction(x,y)] != self.lastState[game.encodeAction(x,y)]):
					xPos, yPos = self.getPiecePos(x, y)
					sprite = scene.SpriteNode('plf:HudCoin' if (state[game.encodeAction(x,y)]==1) else 'plf:Item_CoinSilver')
					sprite.position = (xPos, self.size[1])
					sprite.size = (self.getSize(), self.getSize())
					sprite.run_action(scene.Action.move_to(xPos, yPos, 0.5))
					self.add_child(sprite)
										
					self.sprites.append(sprite)
										
				self.lastState[game.encodeAction(x,y)] = state[game.encodeAction(x,y)]
				
					
scene.run(Connect4GUI())
