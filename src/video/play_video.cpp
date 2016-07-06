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

