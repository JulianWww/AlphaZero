import json
import matplotlib.pyplot as plt
import numpy as np

deltaElo = 105

with open("elos.json", "r") as file:
    vals = json.load(file)

elos = np.array([x for x in vals.values()])
expected = [100 + deltaElo * int(idx) for idx in vals.keys()]

x = np.array([int(x) for x in vals.keys()])
regressionPoints = np.vstack([x, np.ones(len(vals))]).T
m, c = np.linalg.lstsq(regressionPoints, elos, rcond=None)[0]
print(m)

fig = plt.figure()
ax1 = fig.add_subplot(111)
ax1.set_ylabel('Elo-raiting')
ax1.set_xlabel("neural network version")
ax1.set_title('Raiting by version')

ax1.plot(elos, lw=2, label="true Raiting")
ax1.plot(expected, lw=2, label="expected Raiting")
plt.plot(x, m*x + c, 'r', label='fitted Raiting')

plt.legend()

plt.subplots_adjust(left=0.17)
plt.show()
