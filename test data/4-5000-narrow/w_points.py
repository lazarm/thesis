import numpy as np
import pylab as pl
from matplotlib import collections  as mc
import matplotlib.pyplot as plt
import os, sys

filea = open(os.getcwd() + '\\' + sys.argv[1])
a = filea.readlines()

x = [0]*len(a)
y = [0]*len(a)
for i,line in enumerate(a):
    b = line.split(' ')
    x[i] = float(b[0])
    y[i] = float(b[1])

expWidth = int(sys.argv[1].split("-")[1])
expHeight = expWidth/4
plt.scatter(x,y,s=0.9)
#plt.normalize
plt.title('Input points for SSSP tree')
plt.xlim(-0.1,expWidth+0.1)
plt.ylim(-0.1,expHeight+0.1)
plt.xlabel('x')
plt.ylabel('y')
plt.show()
