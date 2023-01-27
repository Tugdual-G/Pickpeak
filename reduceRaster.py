#!/usr/bin/env python3
import numpy as np
from numba import jit


@jit(nopython=True, parallel=True, cache=True)
def max_reduce_nodata(z, h, nodata):
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
            return vmax, int(imax)

        m = z.shape[0]
        n = int(z.shape[1] // h)
        j_in = np.empty(n * z.shape[0], dtype=np.int_)
        i_prev = np.empty(n * z.shape[0], dtype=np.int_)
        zr = np.empty((n, z.shape[0]), dtype=np.double)
        i0 = 0
        for i in range(m):
            for j in range(n):
                zr[j, i], i0 = maxnan(z[i, j * h : (j + 1) * h], nodata)
                j_in[j * m + i] = i0 + j * h
                i_prev[j * m + i] = i_prev0[i * n * h + (j * h + i0)]

        return zr, j_in, i_prev

    m, n = z.shape

    idxj = np.empty((n * m), dtype=np.int_)
    for i in range(m):
        for j in range(n):
            idxj[i * n + j] = i

    idxi = np.arange(n // h * m) - 1
    z, idxj[: n // h * m], _ = red(z, h, idxj, nodata)
    z, idxi[: (m // h) * (n // h)], idxj[: (m // h) * (n // h)] = red(
        z, h, idxj, nodata
    )
    return (
        z,
        idxj[: (m // h) * (n // h)],
        idxi[: (m // h) * (n // h)],
    )


def test1():
    import matplotlib.pyplot as plt
    import time

    pi = np.pi
    t = 0
    m, n = 2**10, 2**10
    h = 2**8
    x, y = np.meshgrid(np.arange(n), np.arange(m))

    r = np.sqrt(x**2 + y**2)
    a = np.sin(pi * 4 * r / m)

    r = np.sqrt((x - 2 * n) ** 2 + y**2)
    a += np.sin(pi * 7 * r / m)

    a[0, 0] = 3
    a[-1, 0] = 3
    a[4, 3] = 3
    plt.pcolormesh(x, y, a)

    t0 = time.time_ns()
    a, idxj, idxi = max_reduce_nodata(a, h, 3)
    t1 = time.time_ns()
    t += t1 - t0
    print(f"time {t / 1e6} ms")

    ax = plt.gca()
    ax.vlines(np.arange(0, n, h) - 1 / 2, -1 / 2, m - 1 / 2, color="k")
    ax.hlines(np.arange(0, m, h) - 1 / 2, -1 / 2, n - 1 / 2, color="k")
    plt.scatter(idxj, idxi, marker="o", c="r")
    plt.show()


def test2():
    import rasterio as rio
    import matplotlib.pyplot as plt
    import time

    dpath = "../topo_map/data/data/raster/BDALTIV2_2-0_25M_ASC_LAMB93-IGN69_D085_2021-09-15/BDALTIV2/1_DONNEES_LIVRAISON_2021-10-00008/BDALTIV2_MNT_25M_ASC_LAMB93_IGN69_D085/"
    f = "BDALTIV2_25M_FXX_0400_6650_MNT_LAMB93_IGN69.asc"

    datasets = rio.open(dpath + f, crs="eps:2154")
    z = datasets.read(1)
    print(np.isfortran(z))
    z = np.array(z, dtype=np.double)
    z = z[::10, ::10]
    m, n = z.shape
    z[60:70, 70:90] = 300
    z[10:30, 50:70] = 9999
    h = 20
    x, y = np.meshgrid(np.arange(n), np.arange(m))
    plt.pcolormesh(x, y, z, vmax=320)
    zr, idxj, idxi = max_reduce_nodata(z, h, 9999)
    for i in range(m // h):
        print(i)
        for j in range(n // h):
            maxv = np.amax(z[h * i : h * (i + 1), h * j : h * (j + 1)])
            print("    ", j, round(maxv), round(zr[i, j]))

    plt.vlines(np.arange(0, n, h) - 1 / 2, -1 / 2, m - 1 / 2, color="k")
    plt.hlines(np.arange(0, m, h) - 1 / 2, -1 / 2, n - 1 / 2, color="k")
    plt.scatter(idxj, idxi, marker="+", c="r")
    plt.show()


if __name__ == "__main__":
    test1()
