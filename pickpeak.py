#!/usr/bin/env python3
import glob
import numpy as np
import rasterio as rio
from rasterio.merge import merge
from rasterio.windows import Window
import matplotlib.pyplot as plt
from numba import jit


@jit(nopython=True, parallel=True, cache=True)
def localmax(R, xs, ys, zs, bbox):
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


@jit(nopython=True, parallel=True, cache=True)
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
            # xmax += [x0]
            # ymax += [y0]
            # maxlist += [lmax]

    xmax = xmax[0:q]
    ymax = ymax[0:q]
    maxlist = maxlist[0:q]
    return (xmax), (ymax), (maxlist)


def find_summits(R, z, bbox=None):
    """R must be in index coordinates"""
    h0 = R // 2
    x, y, z = verylocalmax(z, h0)
    return x, y, z, localmax(R, x, y, z, bbox)


def main():
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
    x0, y0, z0, _ = find_summits(h, z, bbox=bbox)
    importance = np.ones(len(x0))

    for i in range(1, 4):
        idx = localmax(2 * i * h, x0, y0, z0, bbox=bbox)
        importance[idx] = 1 + i

    x0, y0 = rio.transform.xy(trans, y0, x0)

    x, y = np.meshgrid(
        np.arange(z.shape[1]),
        np.arange(z.shape[0]),
    )
    x, y = rio.transform.xy(trans, y, x)
    x, y = np.array(x), np.array(y)

    step = 4
    plt.pcolormesh(x[::step, ::step], y[::step, ::step], z[::step, ::step], cmap="gray")
    plt.scatter(
        x=x0,
        y=y0,
        c=importance.tolist(),
        marker="o",
    )
    plt.show()


main()
