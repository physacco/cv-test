#!/usr/bin/env python3
# encoding: utf-8
# pylint: disable=no-member

"""Play a video with OpenCV."""

import sys
import cv2

def main():
    """The main function of this module."""
    cv2.namedWindow('video', cv2.WINDOW_AUTOSIZE)

    cap = cv2.VideoCapture(0)

    # Set the image resolution.
    # If the camera's resolution is less than 1920x1080,
    # it will use its maximum resolution. Need test.
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)

    i = 0
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:  # done
            break

        i += 1
        if i == 1:
            print frame.shape, frame.dtype, frame.size

        cv2.imshow('video', frame)

        key = cv2.waitKey(30)
        if key & 0xFF == ord('q'):  # quit
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == '__main__':
    main()
