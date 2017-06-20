# a script that reads output text file that is produced by c++ program
# and contains st segment, edges, root, and a pair of points that form a cycle
from matplotlib import pyplot as plt
import os, sys


############### data extraction ################
textFileDir = os.getcwd() + sys.argv[1]

filea = open(textFileDir)
a = filea.readlines()

num_points = int(a[0])
print num_points
points = a[1:(num_points+1)]


rootnum = a[num_points+1] #cifr


num_edges = 0
j = 1
sttext = 'st\n'
while (a[num_points+j] != sttext):
    num_edges = num_edges + 1;
    j = j+1

edges_x_pair = [0]*num_edges
edges_y_pair = [0]*num_edges

for i in range(num_edges-1):
    edge = a[num_points+2+i].split(' - ')
    print edge
    edges_x_pair[i] = [float(edge[0].split(',')[0]), float(edge[0].split(',')[1])]
    edges_y_pair[i] = [float(edge[1].split(',')[0]), float(edge[1].split(',')[1])]

sttext = a[num_points+1+num_edges] #must equal to 'st'

stx = a[num_points+1+num_edges+1]
sty = a[num_points+1+num_edges+2]

cyclex = a[num_points+1+num_edges+3]
cycley = a[num_points+1+num_edges+4]

############# plotting #####################
fig = plt.figure()
ax = fig.add_subplot(111)

points_x = [0]*num_points
points_y = [0]*num_points
for i,point in enumerate(points):
    ppoint = point.split(',')
    points_x[i] = float(ppoint[0])
    points_y[i] = float(ppoint[1])

plt.plot(points_x, points_y, 'ro')
for xy in zip(points_x, points_y):                                                # <--
    ax.annotate('(%s, %s)' % xy, xy=xy, textcoords='offset points')

st_x = [float(stx.split(',')[0]), float(stx.split(',')[1])]
st_y = [float(sty.split(',')[0]), float(sty.split(',')[1])]
plt.plot(st_x,st_y, '-bo', markersize=8)
st = ['s','t']
for sti, x, y in zip(st,st_x, st_y):
    ax.annotate(sti, xy= (x, y), textcoords='offset points', xytext=(-8,8))

for X,Y in zip(edges_x_pair, edges_y_pair):
    plt.plot(X, Y, '-r')

plt.plot(points_x[int(rootnum)], points_y[int(rootnum)], 'go', markersize=10)

cycle_x = [float(cyclex.split(',')[0]), float(cyclex.split(',')[1])]
cycle_y = [float(cycley.split(',')[0]), float(cycley.split(',')[1])]
plt.plot(cycle_x, cycle_y, '--g')

plt.xlim(-0.1, 4)
plt.ylim(-0.1, 4)
plt.show()
