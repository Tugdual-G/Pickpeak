#!/usr/bin/env python3
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.cm as cm

vallist = np.arange(0, 257, 4)
cmap = mpl.cm.inferno
# norm = mpl.colors.Normalize(vmin=0, vmax=256)
colorlist = cmap(vallist)
colorlist = (256 * colorlist[:, :-1]).astype(np.int_)
print(colorlist)
np.savetxt("rgbcmap.asc", colorlist, fmt="%u")
