#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np
import rasterio as rio

data = np.fromfile("binary.double", dtype=np.double)

data.shape = 1000, 1000
plt.imshow(data)
plt.show()

path = "BDALTIV2_25M_FXX_0275_6650_MNT_LAMB93_IGN69.asc"
datasets = rio.open(path, crs="eps:2154")
z = datasets.read(1)
plt.imshow(z)
plt.show()
