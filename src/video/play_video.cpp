// Copyright: This program is released into the public domain.

#include <stdio.h>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: play_video input_file\n");
    return 1;
  }

  cv::VideoCapture cap(argv[1]);  // open the video file
  if (!cap.isOpened()) {  // check if we succeeded
    fprintf(stderr, "Error: failed to open video file\n");
    return 1;
  }

  cv::namedWindow("Video", 1);
  while (true) {
    cv::Mat frame;
    cap >> frame;  // read a new frame
    if (frame.empty()) {  // see [1]
      break;
    }

    cv::imshow("Video", frame);

    int key = cv::waitKey(30);
    if (key == 'q') {  // quit
      break;
    }
  }

  cap.release();
  cv::destroyAllWindows();
  return 0;
}

// [1] The result of cv::imread, cv::VideoCapture::read
// can be determined by the output Mat's properties:
// Success: cols >  0, rows >  0, data != NULL, empty() == 0
// Failure: cols == 0, rows == 0, data == NULL, empty() == 1
