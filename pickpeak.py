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


def find_summits(R, z, bbox=None):
    """R must be in index coordinates"""
    # the max distance possible between two point should
    # be <= R in the first pass
    h0 = int(R / np.sqrt(2))
    if R < 2:
        R = 2
    z0, x, y = max_reduce_nodata(z, h0, -999)
    # return x, y, z0, localmax(R, x, y, z0.ravel(), bbox)
    return z0, x, y


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
    z = np.array(z)
    # z[10:50, 50:60] = 350
    z[-160:-140, -190:-170] = 350
    trans = datasets.transform
    dx = trans[0]

    h = int(1e3 // dx)
    h *= 8
    print(h)
    bbox = (0, z.shape[0], 0, z.shape[1])

    t0 = time.time_ns()
    # z0, x0, y0 = find_summits(h, z, bbox=bbox)
    z0, x0, y0 = max_reduce_nodata(z, h, -9999)
    plt.imshow(z)
    plt.scatter(x0, y0, c="k")
    t1 = time.time_ns()
    print("time", (t1 - t0) / 1e6)

    # z0 = z0.ravel()
    # importance = np.ones(len(x0), dtype=np.int_)
    #
    ## t0 = time.time_ns()
    ## for i in range(1, 6):
    ## idx = localmax(2 * i * h, x0, y0, z0, bbox=bbox)
    ## importance[idx] += 1
    ## t1 = time.time_ns()
    ## print("time", (t1 - t0) / 1e6)
    #
    # x0, y0 = rio.transform.xy(trans, y0, x0)
    ## print(*trans)
    ## x0 = x0 * trans[0] + trans[2]
    ## y0 = y0 * trans[4] + trans[5]
    #
    # x, y = np.meshgrid(
    # np.arange(z.shape[1]),
    # np.arange(z.shape[0]),
    # )
    # x, y = rio.transform.xy(trans, y, x)
    # x, y = np.array(x), np.array(y)
    # x0, y0 = np.array(x0), np.array(y0)
    #
    # step = 4
    # plt.pcolormesh(
    # x[::step, ::step], y[::step, ::step], z[::step, ::step], cmap="viridis"
    # )
    # h0 = int(h / np.sqrt(2))
    # for i in range(1000 // h0 + 1):
    # xl = x[0, 0] + i * h0 * dx
    # plt.axvline(xl)
    #
    # for i in range(1000 // h0 + 1):
    # yl = y[-1, 0] + i * h0 * dx
    # plt.axhline(yl)

    # plt.scatter(
    # x=x0,
    # y=y0,
    # c=importance.tolist(),
    # marker="o",
    # s=(10 * (2 * importance) ** 2).tolist(),
    # )
    # print(x0.shape)
    # plt.scatter(x0, y0)

    plt.show()


main()
