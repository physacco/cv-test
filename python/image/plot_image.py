#!/usr/bin/env python3

import sys
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

img = mpimg.imread(sys.argv[1])
plt.imshow(img)
plt.show()
