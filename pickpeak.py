#!/usr/bin/env python3
import numpy as np
import rasterio as rio
from rasterio.merge import merge
from rasterio.windows import Window
import matplotlib.pyplot as plt


def localmax(R, xs, ys, zs, margins=[0, 0, 0, 0]):
    """R , xs, ys and margin should be intergers in index coordinates"""
    X, Y = np.meshgrid(xs, ys)
    D = (X - X.T) ** 2 + (Y - Y.T) ** 2
    x0 = []
    y0 = []
    z0 = []
    R = R**2
    m, n = X.shape
    left, right, bottom, top = margins

    for i in range(m):
        trigg = True
        for j in range(n):
            if D[i, j] < R and (xs[i] != xs[j] or ys[i] != ys[j]):
                if zs[i] < zs[j]:
                    trigg = False

        if trigg and (left < xs[i] < right) and (bottom < ys[i] < top):
            x0 += [xs[i]]
            y0 += [ys[i]]
            z0 += [zs[i]]

    return x0, y0, z0


def verylocalmax(z, h):
    maxz = np.zeros_like(z) - 9999
    xmax = []
    ymax = []
    maxlist = []
    for i in range(h, z.shape[0] - h, 2 * h):
        for j in range(h, z.shape[1] - h, 2 * h):
            lmax = z[i, j]
            y0, x0 = i, j
            for k in range(-h, h):
                for l in range(-h, h):
                    if lmax < z[i + k, j + l]:
                        lmax = z[i + k, j + l]
                        y0, x0 = i + k, j + l
            maxz[y0, x0] = lmax
            xmax += [x0]
            ymax += [y0]
            maxlist += [lmax]

    for i in range(z.shape[0] - h - 1, h, -2 * h):
        for j in range(z.shape[1] - h - 1, h, -2 * h):
            lmax = z[i, j]
            y0, x0 = i, j
            for k in range(-h, h):
                for l in range(-h, h):
                    if lmax < z[i + k, j + l]:
                        lmax = z[i + k, j + l]
                        y0, x0 = i + k, j + l
            maxz[y0, x0] = lmax
            xmax += [x0]
            ymax += [y0]
            maxlist += [lmax]
    return maxz, np.array(xmax), np.array(ymax), maxlist


def find_summits(h, z, margin=None):
    h0 = h // 2
    maxz, xmax0, ymax0, zs = verylocalmax(z, h0)
    return localmax(h, xmax0, ymax0, zs, margin)


def main():
    fname = "BDALTIV2_25M_FXX_0325_6625_MNT_LAMB93_IGN69.asc"
    dpath = "/home/tugdual/Documents/Programmation/topo_map/data/data/raster/BDALTIV2_2-0_25M_ASC_LAMB93-IGN69_D085_2021-09-15/BDALTIV2/1_DONNEES_LIVRAISON_2021-10-00008/BDALTIV2_MNT_25M_ASC_LAMB93_IGN69_D085/"

    datasets = rio.open(dpath + fname, crs="eps:2154")
    z = datasets.read(1)
    trans = datasets.transform
    dx = trans[0]

    h = int(1e3 // dx)
    h *= 2

    x0, y0, z0 = find_summits(h, z, margin=[h, z.shape[0] - h, h, z.shape[1] - h])
    x0, y0 = rio.transform.xy(trans, y0, x0)

    x, y = np.meshgrid(
        np.arange(z.shape[1]),
        np.arange(z.shape[0]),
    )
    x, y = rio.transform.xy(trans, y, x)
    x, y = np.array(x), np.array(y)
    #
    step = 4
    plt.pcolormesh(x[::step, ::step], y[::step, ::step], z[::step, ::step])
    plt.scatter(x0, y0, marker=".", color="k")
    plt.show()


main()
