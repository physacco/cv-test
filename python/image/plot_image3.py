#!/usr/bin/env python3

import sys
import cv2
import matplotlib.pyplot as plt

bgr_img = cv2.imread(sys.argv[1])

b, g, r = cv2.split(bgr_img)
rgb_img = cv2.merge([r, g, b])

plt.imshow(rgb_img)
plt.xticks([]), plt.yticks([])  # to hide tick values on X and Y axis
plt.show()
