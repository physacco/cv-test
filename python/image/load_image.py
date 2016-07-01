#!/usr/bin/env python3

import sys
import cv2

img_file = sys.argv[1]
color_img = cv2.imread(img_file, cv2.IMREAD_COLOR)     # rgb
alpha_img = cv2.imread(img_file, cv2.IMREAD_UNCHANGED) # rgba
gray_img = cv2.imread(img_file, cv2.IMREAD_GRAYSCALE)  # grayscale

for img in (color_img, alpha_img, gray_img):
    print('type: %s' % type(img))
    print('shape: %s' % repr(img.shape))
    print('dtype: %s' % repr(img.dtype))
    print('size: %s' % repr(img.size))
    print('----------------------------------------')
