import numpy as np
import math
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D


X_MAX_LEN = 3.1416
T_MAX_TIME = 1
h = 0.15
tau = 0.1

M = int(X_MAX_LEN / h)
N = int(T_MAX_TIME / tau)



x_range = np.arange(0, M, h)
t_range = np.arange(0, N, tau)

u = np.zeros((N, M))
with open('output.txt') as file:
    for n in range(N):
        u[n] = np.array([file.readline().split()])

print(N)
print(M)
#print(u)
xgrid, tgrid = np.meshgrid(x_range, t_range)
zgrid = np.sin(xgrid) * np.sin(tgrid) / (xgrid * tgrid)
print(xgrid)
print(tgrid)
print(u.size)
#print(zgrid)
#print(u)
#print(xgrid)

plt.scatter(xgrid, tgrid, s = zgrid)

fig = plt.figure(figsize=(7, 4))
ax_3d = Axes3D(fig)
#plt.minorticks_on()
#plt.grid(which='major', color = 'k', linewidth = 2)
#plt.grid(which='minor', color = 'k', linestyle = ':')
ax_3d.plot_surface(xgrid, tgrid, zgrid)
#ax = fig.add_subplot(111, projection='3d')
#ax.scatter(x_range, t_range, u)
plt.show()