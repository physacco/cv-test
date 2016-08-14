#!/usr/bin/env python3
# encoding: utf-8
# pylint: disable=no-member

"""Add frame number on a video file.

It reads an input video file, print the frame number on each frame,
and save the frames into another video file.
"""

import sys
import time
import argparse
import cv2

def format_time(decimal_seconds):
    """Format seconds into HH:MM:SS.mmm format.
    e.g. 100.5 => 00:01:40.500
    """
    seconds = int(decimal_seconds)
    milliseconds = int((decimal_seconds - seconds) * 1000)
    hours = seconds // 3600
    minutes = (seconds % 3600) // 60
    seconds = seconds % 60
    return '%d:%02d:%02d.%03d' % (hours, minutes, seconds, milliseconds)

def add_frame_number(img, frame_number):
    """Add frame number on the image.
    Arguments:
      img: the image, an instance of numpy.ndarray
      frame_number: the frame number, an integer
      width: width of the image in pixels, an integer
    Returns None.
    """
    text = '% 8d' % frame_number
    width = img.shape[1]
    cv2.putText(img, text, (width - 180, 50), cv2.FONT_HERSHEY_SIMPLEX,
                1, (0, 0, 255), 2, cv2.LINE_AA)

def show_image(img):
    """Display the image.
    It will raise a SystemExit if the user has pressed Q.
    """
    cv2.imshow('video', img)
    key = cv2.waitKey(1) & 0xff
    if key == ord('q'):  # quit
        raise SystemExit

def main_loop(cap, wrt, watch):
    """Process the video, frame by frame.
    If watch is True, play the video while converting.
    """
    frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))

    start_time = time.time()
    index = 0
    while True:
        ret, img = cap.read()
        if not ret:
            break

        add_frame_number(img, index)
        wrt.write(img)

        if watch:
            try:
                show_image(img)
            except SystemExit:
                break

        index += 1
        curr_time = time.time()
        elapsed = curr_time - start_time
        remain = elapsed * (frames - index) / index
        percent = 100.0 * index / frames
        sys.stdout.write('%d/%d %.2f%% %s\r' % \
            (index, frames, percent, format_time(remain)))

def process_file(input_file, output_file, watch=False):
    """Process the input file, write to the output file.
    If watch is True, play the video while converting.
    """
    cap = cv2.VideoCapture(input_file)
    if not cap.isOpened():
        sys.stderr.write('Error: cannot open input file\n')
        sys.exit(1)

    width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    wrt = cv2.VideoWriter(output_file,
                          int(cap.get(cv2.CAP_PROP_FOURCC)),
                          cap.get(cv2.CAP_PROP_FPS),
                          (width, height))

    main_loop(cap, wrt, watch)

    cap.release()
    wrt.release()

def main():
    """The main function of this module."""
    parser = argparse.ArgumentParser()
    parser.add_argument('--watch', action='store_true',
                        help='play the video while converting')
    parser.add_argument('input', type=str, help='input video file')
    parser.add_argument('output', type=str, help='output video file')
    args = parser.parse_args()
    process_file(args.input, args.output, watch=args.watch)

if __name__ == '__main__':
    main()
