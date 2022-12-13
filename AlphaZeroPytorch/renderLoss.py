import json
import numpy as np
import matplotlib.pyplot as plt

with open("logs/games/loss.json") as file:
    data = np.array(json.load(file))

plt.plot(data[:,0,0])
plt.legend(["value head loss"])
plt.show()
