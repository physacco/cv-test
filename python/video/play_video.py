#!/usr/bin/env python3
# encoding: utf-8

import sys
import cv2

cv2.namedWindow('video', cv2.WINDOW_AUTOSIZE)

cap = cv2.VideoCapture(sys.argv[1])
while cap.isOpened():
    ret, frame = cap.read()
    if not ret:  # done
        break

    cv2.imshow('video', frame)

    key = cv2.waitKey(30)
    if key & 0xFF == ord('q'):  # quit
        break

cap.release()
cv2.destroyAllWindows()
