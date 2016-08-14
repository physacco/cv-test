#!/usr/bin/env python3
# encoding: utf-8
# pylint: disable=no-member

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
    """This class is used to manage capture properties."""
    def __init__(self, cap):
        self.fps = cap.get(cv2.CAP_PROP_FPS)
        self.fourcc = self.parse_fourcc(cap.get(cv2.CAP_PROP_FOURCC))
        self.frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
        self.frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        self.frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))

    def show(self):
        """Print capture properties to stdout."""
        print('FPS: %.3f' % self.fps)
        print('FourCC: %s' % self.fourcc)
        print('Frame Count: %d' % self.frame_count)
        print('Frame Width: %d' % self.frame_width)
        print('Frame Height: %d' % self.frame_height)

    @staticmethod
    def parse_fourcc(value):
        """Parse cv2.CAP_PROP_FOURCC into a 4-character string.
        The value obtained by OpenCV is a float number.
        """
        value = int(value)
        cc0 = value & 0xff
        cc1 = (value >> 8) & 0xff
        cc2 = (value >> 16) & 0xff
        cc3 = (value >> 24) & 0xff
        chars = [chr(i) for i in [cc0, cc1, cc2, cc3]]
        return ''.join(chars)

def main():
    """The main function of this module."""
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

if __name__ == '__main__':
    main()
