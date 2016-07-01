#!/usr/bin/env python3

import sys
import cv2

bgr_img = cv2.imread(sys.argv[1])
gray_img = cv2.cvtColor(bgr_img, cv2.COLOR_BGR2GRAY)
cv2.imwrite('output.jpg', gray_img)
