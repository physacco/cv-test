#!/usr/bin/env python3

import sys
import cv2
import matplotlib.pyplot as plt

bgr_img = cv2.imread(sys.argv[1])
rgb_img = cv2.cvtColor(bgr_img, cv2.COLOR_BGR2RGB)
plt.imshow(rgb_img)
plt.xticks([]), plt.yticks([])  # to hide tick values on X and Y axis
plt.show()
