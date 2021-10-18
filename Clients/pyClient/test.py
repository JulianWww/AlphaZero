import random
import math

def fun():
    return 2

def _eval(x):
    print(x)
    print(type(x))

for x in range(13):
   print(_eval([fun() for x in range(8)]))
