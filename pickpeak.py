#!/usr/bin/env python3
from logging import raiseExceptions
from matplotlib import markers
import numpy as np
from numba import jit
from reduceRaster import max_reduce_nodata
import pdb


@jit(nopython=True, parallel=True, cache=True)
def localmax(R, xs, ys, zs, bbox):
    """
    Find the isolated peak and return ok_idx, i.e where to find their
    position in xs, ys.

    R , xs, ys and bbox should be integers in index coordinates
    """
    idx = np.arange(len(xs) + 1)
    ok_idx = np.zeros((len(xs)), dtype=np.int_) - 1
    R = R**2
    m = len(xs)
    k = 0
    i = 0
    while i < m:
        trigg = True
        for j in range(m):
            d = (xs[i] - xs[j]) ** 2 + (ys[i] - ys[j]) ** 2
            if d < R:
                # if the summits are of same z, we keep only one (i)
                if zs[i] < zs[j]:
                    trigg = False
                    break
                else:
                    # we trow away the value of idx[j]
                    # because we don't have to test it against another summit
                    idx[j] = idx[j + 1]

        if trigg and (bbox[0] <= xs[i] < bbox[1]) and (bbox[2] <= ys[i] < bbox[3]):
            ok_idx[k] = i
            k += 1
        i += 1
        # going through the discarded values until
        # finding a potentially valid one
        while i != idx[i]:
            i = idx[i]

    return ok_idx[0:k]


def find_summits(h, z, bbox=None, nodata=-9999):
    """
    h must be in index coordinates
    the max distance possible between two summits, is sqrt(2)*h
    """

    if not isinstance(h, int):
        raise TypeError(f"h should be an integer not {type(h)}")

    if bbox == None:
        bbox = (0, z.shape[0], 0, z.shape[1])

    R = h * np.sqrt(2)
    z0, x, y = max_reduce_nodata(z, h, nodata)
    z0.shape = z0.shape[0] * z0.shape[1]

    return z0, x, y, localmax(R, x, y, z0, bbox)


def main():
    from matplotlib.patches import Circle
    import rasterio as rio
    from rasterio.merge import merge
    from rasterio.windows import Window
    import matplotlib.pyplot as plt
    import time
    import glob

    dpath = "../topo_map/data/data/raster/BDALTIV2_2-0_25M_ASC_LAMB93-IGN69_D085_2021-09-15/BDALTIV2/1_DONNEES_LIVRAISON_2021-10-00008/BDALTIV2_MNT_25M_ASC_LAMB93_IGN69_D085/"

    f_paths = glob.glob(dpath + "*.asc")
    f = "BDALTIV2_25M_FXX_0400_6650_MNT_LAMB93_IGN69.asc"

    print(f)
    datasets = rio.open(dpath + f, crs="eps:2154")
    z = datasets.read(1)
    z = z[::, ::]
    # z = np.array(z, dtype=np.double)

    m, n = z.shape
    h = 100
    print(f"h0={h}")
    R = h * np.sqrt(2)
    print(f"R={R}")

    t0 = time.time_ns()
    zr, x0, y0, idx = find_summits(h, z)
    t1 = time.time_ns()
    print(f"time {(t1 - t0) / 1e6} ms")
    imp = np.ones_like(x0)
    imp[idx] = 2

    x, y = np.meshgrid(np.arange(n), np.arange(m))
    plt.pcolormesh(x, y, z, cmap="gist_earth")
    plt.vlines(np.arange(0, n, h) - 1 / 2, -1 / 2, m - 1 / 2, color="k", alpha=0.3)
    plt.hlines(np.arange(0, m, h) - 1 / 2, -1 / 2, n - 1 / 2, color="k", alpha=0.3)

    plt.scatter(
        x0,
        y0,
        c="k",
        marker=".",
        s=50,
    )

    plt.scatter(
        x0[idx],
        y0[idx],
        c="k",
        marker="+",
        s=350,
    )

    ax = plt.gca()
    fig = plt.gcf()
    for xc, yc in zip(x0[idx], y0[idx]):
        circ = Circle((xc, yc), R, fill=False, edgecolor="k", linestyle="--")
        ax.add_patch(circ)
    ax.set_aspect("equal")
    ax.set_xlim(0, n)
    ax.set_ylim(0, m)

    # fig.set_size_inches(8, 8)
    # plt.savefig("test.png", bbox_inches="tight", dpi=100)

    plt.show()


if __name__ == "__main__":
    main()
