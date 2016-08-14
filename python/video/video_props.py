#!/usr/bin/env python3
# encoding: utf-8

"""Show video info obtained by OpenCV.

References:
  * Reading and Writing Video
    http://docs.opencv.org/3.0-alpha/modules/videoio/doc/reading_and_writing_video.html
  * Video Codecs by FOURCC
    http://www.fourcc.org/codecs.php
"""

import sys
import cv2

class CaptureProperties(object):
    def __init__(self, cap):
        self.fps = cap.get(cv2.CAP_PROP_FPS)
        self.fourcc = self.parseFourCC(cap.get(cv2.CAP_PROP_FOURCC))
        self.frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
        self.frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        self.frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))

    def show(self):
        print('FPS: %.3f' % self.fps)
        print('FourCC: %s' % self.fourcc)
        print('Frame Count: %d' % self.frame_count)
        print('Frame Width: %d' % self.frame_width)
        print('Frame Height: %d' % self.frame_height)

    @staticmethod
    def parseFourCC(value):
        value = int(value)
        a = value & 0xff
        b = (value >> 8) & 0xff
        c = (value >> 16) & 0xff
        d = (value >> 24) & 0xff
        return ''.join([chr(i) for i in [a, b, c, d]])

if __name__ == '__main__':
    file_count = len(sys.argv) - 1
    if file_count < 1:
        sys.stderr.write('Usage: %s file1 [file2 ...]\n' % sys.argv[0])
        sys.exit(1)

    for i in range(file_count):
        filename = sys.argv[i + 1]
        print('File: %s' % filename)

        cap = cv2.VideoCapture(filename)
        if not cap.isOpened():
            continue

        props = CaptureProperties(cap)
        props.show()
        if file_count > 1 and i < file_count - 1:
            print('-' * 30)
