// Copyright: This program is released into the public domain.

#include <stdio.h>
#include <opencv2/opencv.hpp>

int main() {
  cv::VideoCapture cap(0);  // open the default camera
  if (!cap.isOpened()) {  // check if we succeeded
    fprintf(stderr, "Error: failed to open camera\n");
    return 1;
  }

  cv::namedWindow("Camera", 1);
  while (true) {
    cv::Mat frame;
    cap >> frame;  // read a new frame
    if (frame.empty()) {
      break;
    }

    cv::imshow("Camera", frame);

    int key = cv::waitKey(30);
    if (key == 'q') {  // quit
      break;
    }
  }

  cap.release();
  cv::destroyAllWindows();
  return 0;
}
