import tkinter as tk
from PIL import Image, ImageTk
from game import Game
from threading import Thread
import time

class ConsoleAgent:
    """Agent running in the console for testing only"""
    def render(self, state):
        "render the state to the console" 
        state.consoleRender()

    def getAction(self, state):
        "get the Action the player wants to perform function will be called until valid output is found"
        return state.actionModifier(int(input("your Action: ")))

    def winScreen(self, state):
        "dummy for now"
        pass


class GUI(tk.Tk):
    """
    render game to GUI using Tkinter and canvas
    """
    colorMap = {
        1: "gold",
        -1:"red",
        0: "white"
    }
    yPadRel = 0.1
    _canvasy = 450
    _canvasx = 500
    _dotSize = 0.45
    _lastState = None
    _win = 0
    _winLinesRendered = False
    winLines_kwargs = {
        "fill": "#00FF00",
        "width": 10
    }
    def __init__(self):
        super(GUI, self).__init__()
        self.title("Connect4 AlphaZero Client")
        self.geometry("500x500")
        self.bind("<Configure> ", self._resize)

        self.yPad = 60
        self.action = -1

        self.canvas = tk.Canvas(self, height=self._canvasy, width=self._canvasx, bg="#FFFFFF")
        self.canvas.bind("<Button-1>", self._writeAction)
        self.canvas.place(x=0, y=self.yPad)

        self.playerLabel = tk.Label(self, text="testText",font=("Arial", self.yPad//2))
        self.playerLabel.place(x=0, y=0)
                                 
        self._drawBoard()

    def _resize(self, event):
        """callback for resizing of the window"""
        if event.widget == self:
            self.yPad = int(self.yPadRel * event.width)
            self.canvas.place(x=0, y=self.yPad)
            self.playerLabel.config(font=("Arial", self.yPad//2))

            
            self._canvasy = event.height - self.yPad
            self._canvasx = event.width

            self.canvas.config(height=self._canvasy, width=self._canvasx)
            self.render(self._lastState)

    def _getDxDy(self):
        "get the dx and dy neded internaly to compute field and stone sizes"
        return self._canvasx / 8, self._canvasy / 7

    def render(self, state):
        "render the state"
        self._drawBoard()
        if not state is None:
            self._lastState = state
            self._drawStones(state.board)

            if state.player == 1:
                self.playerLabel.config(text = "Yellow's Turn", fg="#808080")
            else:
                self.playerLabel.config(text = "Red's Turn", fg="#808080")

            self.renderWinLines(state)

        if not self._lastState is None:
            if self._lastState.isDone:
                self._renderEndMsg()

        
    def _drawBoard(self):
        "render 7x6 board using lines"
        self.canvas.delete("all")

        dx, dy = self._getDxDy()
        ofset = 0.5
        for x in range(8):
            self.canvas.create_line(dx*(x+ofset), dy*ofset, dx*(x+ofset), self._canvasy - dy*ofset)

        for y in range(7):
            self.canvas.create_line(dx*ofset, dy*(y+ofset), self._canvasx - dx*ofset, dy*(y+ofset))

    def _drawStones(self, state):
        "place stones in board"
        dx, dy = self._getDxDy()

        for x in range(1, 8):
            for y in range(1, 7):
                if state[Game.encodeAction(x-1, y-1)] != 0:
                    Xpos = dx * x
                    Ypos = dy * y
                    Ysize= self._dotSize * dy
                    Xsize= self._dotSize * dx

                    color = self.colorMap[state[Game.encodeAction(x-1, y-1)]]
                    
                    self.canvas.create_oval(
                        Xpos - Xsize, Ypos-Ysize,
                        Xpos+Xsize, Ypos+Ysize,
                        fill=color, width=0
                    )

    def _renderEndMsg(self):
        "render the message at the end of the game"
        args = (self._canvasx//2, self._canvasy//2)
        fontSize = min(self._canvasx//10, self._canvasy//2)
        kwargs = {
            "font": f"Times {fontSize} bold",
            "anchor": "c",
        }
        if self._win == 1:
            self.canvas.create_text(*args, **kwargs, fill="green",
                                    text="You Win")
        elif self._win == -1:
            self.canvas.create_text(*args, **kwargs, fill="red", text="You Loose")

        elif self._win == 0:
            self.canvas.create_text(*args, **kwargs, fill="black", text="Tie")
        
            

    def _writeAction(self, event):
        """
        calleback from canvas mouse left click.
        Converts postion to grid position and than to action witch is saved.
        """
        dx, dy = self._getDxDy()

        XPos = (event.x - dx * 0.5) // dx
        YPos = (event.y - dy * 0.5) // dy

        self.action = int(XPos + 7*YPos)

    def getAction(self, state):
        """Make playerLable black and wait for an action to be written."""
        self.playerLabel.config(fg="#000000")
        self.action = -1
        while self.action == -1:
            time.sleep(0.1)
        return self.action

    def drawLineOverTime(self, x1, y1, x2, y2, steps, dt, args=(), **kwargs):
        "draw a line from (x1, y1) to (x2, y2) over time"
        line = self.canvas.create_line(x1, y1, x1, y1, *args, **kwargs)
        dx = (x2 - x1) / steps
        dy = (y2 - y1) / steps
        for idx in range(steps+1):
            time.sleep(dt)
            self.canvas.delete(line)
            line = self.canvas.create_line(x1, y1, x1+dx*idx, y1+dy*idx, *args, **kwargs)

    def getPos(self, pos):
        "get action to canvas postion"
        a, b = Game.decodeAction(pos)
        dx, dy = self._getDxDy()
        return (a+1)*dx, (b+1)*dy

    def winScreen(self, game, _win):
        "show win screen"
        self._win = 2
        self.render(game)
        self._winLinesRendered = False
        
        dx, dy = self._getDxDy()
        threads = []
        if not game is None:
            for a, b in game.ends:
                x1, y1 = self.getPos(a)
                x2, y2 = self.getPos(b)
                currentThread = Thread(
                    target=self.drawLineOverTime,
                    args=(
                        x1,y1,
                        x2,y2,
                        20,0.01
                    ),
                    kwargs = self.winLines_kwargs
                )
                currentThread.start()
                threads.append(currentThread)

            for thread in threads:
                thread.join()
            del threads

        self._win = _win
        self._winLinesRendered = True

    def renderWinLines(self, game):
        if self._winLinesRendered:
            if game.isDone:
                for a, b in game.ends:
                    x1, y1 = self.getPos(a)
                    x2, y2 = self.getPos(b)
                    self.canvas.create_line(x1,y1,x2,y2, **self.winLines_kwargs)
