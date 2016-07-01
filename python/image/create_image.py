#!/usr/bin/env python3

import sys
import cv2
import numpy as np

# create an image
width, height = 640, 480
img = np.zeros((height, width, 4), np.uint8)
for i in range(height):
    for j in range(width):
        r = int(1.0 * j / (width - 1) * 255)
        g = int(1.0 * i / (height - 1) * 255)
        b = 255
        a = int((r + g) * 0.5)
        img[i, j] = (b, g, r, a)

# save the image
cv2.imwrite('output.png', img)

# display the image
cv2.namedWindow('image', cv2.WINDOW_NORMAL)
cv2.imshow('image', img)
cv2.waitKey(0)
cv2.destroyAllWindows()
