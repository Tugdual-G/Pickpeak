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


# @jit(nopython=True, parallel=True, cache=True)
def max_reduce_nanidx(z, h, nodata, depht=1):
    """Flip Flop"""

    def red(z, h, i_prev0, nodata):
        # This is a reduction fonction,
        # it divide the number of columns by h
        #
        # This fonction takes an array z and process each row by
        # taking the max value in the h first columns, then jump h
        # columns and reapeat.
        #
        # The max of each batch of cell is written to a transposed array zr,
        # index information are transposed to.
        #
        # i_prev0 has the same shape as z, it carries the orginal index i of z
        # before the first time z was processed. Thus no information is lost
        # on the position of the max values.
        #
        # The best case is: z % h = 0
        # in this case some max values could be skiped at the edge.
        #
        #
        #
        #
        def maxnan(x, nodata):
            # Return the max of 1d array ignoring nodata cells
            i = 0
            while x[i] == nodata and i < len(x):
                i += 1
            vmax = x[i]
            imax = i
            for i in range(len(x)):
                if vmax < x[i] and x[i] != nodata:
                    vmax = x[i]
                    imax = i
            return vmax, imax

        m = z.shape[0]
        n = int(z.shape[1] // h)
        j_in = np.arange(n * z.shape[0], dtype=np.int_) - 1
        i_prev = np.arange(n * z.shape[0], dtype=np.int_) - 1
        zr = np.zeros((n, z.shape[0]), dtype=np.double) - 1
        i0 = 0
        for i in range(m):
            for j in range(n):
                zr[j, i], i0 = maxnan(z[i, j * h : (j + 1) * h], nodata)
                j_in[j * m + i] = i0 + j * h
                i_prev[j * m + i] = i_prev0[i * n * h + (j * h + i0)]

        return zr, j_in, i_prev

    m, n = z.shape
    idxj0 = np.ones(n)[np.newaxis, :] * np.arange(m)[:, np.newaxis]
    idxj0 = idxj0.ravel()

    for i in range(depht):
        z, idxj, idxi = red(z, h, idxj0, nodata)
        z, idxj, idxi = red(z, h, idxj, nodata)

    return z, idxj, idxi


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
    m, n = 10, 8
    x, y = np.meshgrid(np.arange(n), np.arange(m))
    a = np.random.rand(m, n)
    a[0, 0] = 3
    a[-1, 0] = 1
    a[4, 3] = 3
    plt.pcolormesh(x, y, a)
    a, idxj, idxi = max_reduce_nanidx(a, h, 3)
    plt.pcolormesh(
        x[::h, ::h] + 0.5,
        y[::h, ::h] + 0.5,
        a * 0,
        cmap="gray",
        alpha=0.4,
        edgecolor="k",
    )
    ax = plt.gca()
    plt.scatter(idxi, idxj)
    plt.show()


main()
