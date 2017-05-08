#!/usr/bin/env python3
# encoding: utf-8
# pylint: disable=no-member

"""Play a video with OpenCV."""

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
    cap.set(cv2.CAP_PROP_FPS, 60.0)

    width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    fps = cap.get(cv2.CAP_PROP_FPS)

    output_file = 'output.mp4'
    fourcc = cv2.VideoWriter_fourcc(*'X264')
    wrt = cv2.VideoWriter(output_file, fourcc, fps, (width, height))

    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:  # done
            break

        wrt.write(frame)

        cv2.imshow('video', frame)

        key = cv2.waitKey(30) & 0xFF
        if key == 27 or key == ord('q'):  # ESC, quit
            break

    cap.release()
    wrt.release()
    cv2.destroyAllWindows()

if __name__ == '__main__':
    main()
