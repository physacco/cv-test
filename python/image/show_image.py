#!/usr/bin/env python3

import sys
import cv2

img = cv2.imread(sys.argv[1], cv2.IMREAD_UNCHANGED)
cv2.namedWindow('image', cv2.WINDOW_NORMAL)
cv2.imshow('image', img)
cv2.waitKey(0)
cv2.destroyAllWindows()
