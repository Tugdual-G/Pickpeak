#!/usr/bin/env python3
import numpy as np
from numba import jit


@jit(nopython=True, parallel=True, cache=True)
def max_reduce(z, h):
    """Flip Flop"""

    def red(z, h):
        n = int(z.shape[1] // h)
        zr = np.zeros((n, z.shape[0]), dtype=np.double) - 1
        for i in range(z.shape[0]):
            for j in range(n):
                zr[j, i] = np.amax(z[i, j * h : (j + 1) * h])

        return zr

    z0 = red(z, h)
    zr = red(z0, h)
    return zr


@jit(nopython=True, parallel=True, cache=True)
def max_reduce_nan(z, h, nodata):
    """Flip Flop"""

    def maxnan(x, nodata):
        i = 0
        while x[i] == nodata and i < len(x):
            i += 1
        vmax = x[i]
        for i in range(len(x)):
            if vmax < x[i] and x[i] != nodata:
                vmax = x[i]
        return vmax

    def red(z, h):
        n = int(z.shape[1] // h)
        zr = np.zeros((n, z.shape[0]), dtype=np.double) - 1
        for i in range(z.shape[0]):
            for j in range(n):
                zr[j, i] = maxnan(z[i, j * h : (j + 1) * h], nodata)

        return zr

    z0 = red(z, h)
    zr = red(z0, h)
    return zr


def main():
    import matplotlib.pyplot as plt

    h = 2
    m, n = 8, 10
    x, y = np.meshgrid(np.arange(n), np.arange(m))
    a = np.random.rand(m, n)
    a[0, 0] = 3
    a[-1, 0] = 2
    plt.pcolormesh(x, y, a)
    plt.show()
    a = max_reduce_nan(a, h, 3)
    plt.pcolormesh(x[::h, ::h], y[::h, ::h], a)
    plt.show()


main()
