import json
import matplotlib.pyplot as plt
import numpy as np

step = [12, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1, 8, 7, 5, 1, 7, 25, 1, 4,
        2, 2, 1, 7, 5, 4, 2, 3, 1, 2, 2, 2, 2, 5, 3, 6, 2, 14, 1, 2, 15, 4, 1, 7, 2, 3, 2, 5, 3, 2, 15, 9, 19,
        5, 11, 3, 4, 1, 2, 2, 5, 2, 3, 3, 5, 2, 6, 7, 2, 2, 4, 3, 14, 2, 1, 2, 3, 4, 5, 2, 2, 1, 2, 5, 3, 2,
        3, 2, 1, 4, 3, 2, 1, 4, 2, 3, 2, 1, 4, 2, 8, 4, 2, 2, 1, 5, 7, 2, 5, 2, 2, 3, 5, 4, 3, 2, 1, 6, 1, 4,
        2, 1, 2, 2, 2, 3, 2, 1, 5, 2, 2, 2, 1, 5, 2, 2, 2, 1, 2, 6, 3, 2, 1, 2, 2, 2, 2, 1, 4, 7, 5, 2, 1, 4,
        3, 2, 2, 1, 6, 3, 2, 2, 2, 3, 7, 2, 7, 1, 2, 2, 3, 5, 2, 3, 2, 3, 3, 4, 1, 4, 1, 2, 2, 5, 7, 2, 2, 3,
        2, 5, 2, 3, 2, 5, 9, 1, 2, 3, 2, 2, 4, 3, 3, 2, 2, 2, 10, 3, 2, 1, 4, 2, 1, 2, 3, 2, 3, 4, 1, 4, 3, 2,
        2, 3, 7, 2, 1, 2, 2, 1, 4, 1, 2, 2, 2, 3, 2, 1, 2, 2, 1, 2, 2, 1, 2, 2, 7, 5, 12, 1, 2, 2, 1, 4, 7, 1,
        5, 2, 2, 2, 6, 2, 2, 2, 7, 2, 5, 5, 4, 7, 3, 4, 1, 4, 1, 2, 3, 2, 2, 3, 3, 7, 1, 2, 8, 2, 1, 2, 2, 1,
        2, 2, 1, 2, 1, 4, 3, 2, 1, 2, 2, 1, 4, 1, 2, 2, 5, 3, 3, 3, 4, 4, 4, 7, 6, 2, 2, 2, 1, 2, 3, 2, 2, 1,
        2, 1, 4, 2]
step = np.array(step)+1

with open("elos.json", "r") as file:
    vals = json.load(file)

elos = []
for idx, val in enumerate(step):
    elo1 = vals[str(idx)]
    elo2 = vals[str(idx + 1)]
    deltaElo = (elo2 - elo1)/val
    for i in range(val):
        elos.append(elo1 + deltaElo * i)

plt.plot(elos)
#plt.plot(vals)
plt.show()