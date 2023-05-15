
import numpy as np
import matplotlib.pyplot as plt
import math

x_range_1 = np.array([100, 1000, 10000, 100000, 1000000, 10000000])
y_range_1 = np.array([375, 5995, 161926, 1928977, 18014988, 298469582])

plt.yscale('log')
plt.xscale('log')

plt.plot(x_range_1, y_range_1)
plt.title("1 thread")
plt.show()

x_range_2 = np.array([100, 1000, 10000, 100000, 1000000])
y_range_2 = np.array([2572, 43271, 1225860, 12840385, 116982757])

plt.yscale('log')
plt.xscale('log')

plt.plot(x_range_2, y_range_2)
plt.title("2 thread")
plt.show()

x_range_4 = np.array([100, 1000, 10000, 100000, 1000000])
y_range_4 = np.array([3915, 64443, 1000786, 10419448, 99249047])


plt.yscale('log')
plt.xscale('log')

plt.plot(x_range_4, y_range_4)
plt.title("4 thread")
plt.show()