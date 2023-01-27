#!/usr/bin/env python3
import numpy as np
from numba import jit
from reduceRaster import max_reduce_nodata


@jit(nopython=True, parallel=True, cache=True)
def localmax(R, xs, ys, zs, bbox):
    """R , xs, ys and margin should be intergers in index coordinates"""
    print(10 * "--")
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
                # exclude identical points with 0<d
                if zs[i] < zs[j] and 0 < d:
                    trigg = False
                    break
                else:
                    idx[j] = idx[j + 1]

        if trigg and (bbox[0] < xs[i] < bbox[1]) and (bbox[2] < ys[i] < bbox[3]):
            ok_idx[k] = i
            k += 1
        i += 1
        while i != idx[i]:
            i = idx[i]

    return ok_idx[0:k]


def find_summits(R, z, bbox, nodata=-9999):
    """R must be in index coordinates"""
    # the max distance possible between two point should
    # be <= R in the first pass
    h0 = int(R / np.sqrt(2))
    if h0 < 2:
        h0 = 2
    z0, x, y = max_reduce_nodata(z, h0, nodata)
    z0.shape = z0.shape[0] * z0.shape[1]
    return z0, x, y, localmax(R, x, y, z0, bbox)


def main():

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
    # z = np.array(z, dtype=np.double)

    m, n = z.shape
    R = 200
    h0 = int(R / np.sqrt(2))
    if h0 < 2:
        h0 = 2

    x, y = np.meshgrid(np.arange(n), np.arange(m))

    bbox = (0, n, 0, m)

    t0 = time.time_ns()
    zr, x0, y0, idx = find_summits(R, z, bbox)
    t1 = time.time_ns()

    imp = np.zeros_like(x0)
    imp[idx] = 1

    print("time", (t1 - t0) / 1e6)

    plt.pcolormesh(x, y, z)
    plt.vlines(np.arange(0, n, h0) - 1 / 2, -1 / 2, m - 1 / 2, color="k")
    plt.hlines(np.arange(0, m, h0) - 1 / 2, -1 / 2, n - 1 / 2, color="k")
    plt.scatter(
        x0, y0, c=imp.tolist(), s=(20 * 2**imp).tolist(), marker="o", cmap="magma"
    )

    plt.show()


main()
