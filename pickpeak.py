#!/usr/bin/env python3
import numpy as np
from numba import jit


@jit(nopython=True, parallel=True, cache=True)
def localmax_simple(R, xs, ys, zs, bbox):
    """R , xs, ys and margin should be intergers in index coordinates"""
    idx = np.zeros((len(xs)), dtype=np.int_) - 1
    R = R**2
    m = len(xs)
    k = 0

    for i in range(m):
        trigg = True
        for j in range(m):
            d = (xs[i] - xs[j]) ** 2 + (ys[i] - ys[j]) ** 2
            if d < R and zs[i] < zs[j]:
                trigg = False
                break

        if trigg and (bbox[0] < xs[i] < bbox[1]) and (bbox[2] < ys[i] < bbox[3]):
            idx[k] = i
            k += 1

    return idx[0:k]


# @jit(nopython=True, parallel=True, cache=True)
def localmax(R, xs, ys, zs, bbox):
    """R , xs, ys and margin should be intergers in index coordinates"""
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


# @jit(nopython=True, parallel=True, cache=True)
def verylocalmax(z, h):
    xmax = np.zeros(int(z.shape[0] * z.shape[1] // 3), dtype=np.int_) - 1
    ymax = np.zeros(int(z.shape[0] * z.shape[1] // 3), dtype=np.int_) - 1
    maxlist = np.zeros(int(z.shape[0] * z.shape[1] // 3), dtype=np.double) - 1
    # xmax = []
    # ymax = []
    # maxlist = []
    q = 0
    for i in range(h, z.shape[0] - h, 2 * h):
        for j in range(h, z.shape[1] - h, 2 * h):
            lmax = z[i, j]
            y0, x0 = i, j
            for k in range(-h, h):
                for l in range(-h, h):
                    if lmax < z[i + k, j + l]:
                        lmax = z[i + k, j + l]
                        y0, x0 = i + k, j + l

            xmax[q] = x0
            ymax[q] = y0
            maxlist[q] = lmax
            q += 1
            # xmax += [x0]
            # ymax += [y0]
            # maxlist += [lmax]

    for i in range(z.shape[0] - h - 1, h, -2 * h):
        for j in range(z.shape[1] - h - 1, h, -2 * h):
            lmax = z[i, j]
            y0, x0 = i, j
            for k in range(-h, h):
                for l in range(-h, h):
                    if lmax < z[i + k, j + l]:
                        lmax = z[i + k, j + l]
                        y0, x0 = i + k, j + l
            xmax[q] = x0
            ymax[q] = y0
            maxlist[q] = lmax
            q += 1
    xmax = xmax[0:q]
    ymax = ymax[0:q]
    maxlist = maxlist[0:q]
    return (xmax), (ymax), (maxlist)


def find_summits(R, z, bbox=None):
    """R must be in index coordinates"""
    # the max distance possible between two point should
    # be <= R in the first pass
    h0 = int(R / np.sqrt(2))
    x, y, z = verylocalmax(z, h0)
    return x, y, z, localmax(R, x, y, z, bbox)


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
    trans = datasets.transform
    dx = trans[0]

    h = int(1e3 // dx)
    h *= 1
    bbox = (h, z.shape[0] - h, h, z.shape[1] - h)

    t0 = time.time_ns()
    x0, y0, z0, idx = find_summits(h, z, bbox=bbox)
    t1 = time.time_ns()
    print("time", (t1 - t0) / 1e6)

    importance = np.ones(len(x0), dtype=np.int_)

    t0 = time.time_ns()
    for i in range(1, 6):
        idx = localmax(2 * i * h, x0, y0, z0, bbox=bbox)
        importance[idx] += 1
    t1 = time.time_ns()
    print("time", (t1 - t0) / 1e6)

    x0, y0 = rio.transform.xy(trans, y0, x0)

    x, y = np.meshgrid(
        np.arange(z.shape[1]),
        np.arange(z.shape[0]),
    )
    x, y = rio.transform.xy(trans, y, x)
    x, y = np.array(x), np.array(y)
    x0, y0 = np.array(x0), np.array(y0)

    step = 4
    plt.pcolormesh(x[::step, ::step], y[::step, ::step], z[::step, ::step], cmap="gray")
    plt.scatter(
        x=x0,
        y=y0,
        c=importance.tolist(),
        marker="o",
        s=(10 * (2 * importance) ** 2).tolist(),
    )

    plt.show()


main()
