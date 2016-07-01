// Copyright: This program is released into the public domain.

#include <stdio.h>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char** argv) {
  // load the source image
  cv::Mat src = cv::imread(argv[1], cv::IMREAD_COLOR);
  if (src.empty()) {
    std::cerr << "Usage: histeq <path_to_image>" << std::endl;
    return 1;
  }

  // convert
  cv::cvtColor(src, src, CV_BGR2GRAY);

  // histogram equalization
  cv::Mat dst;
  cv::equalizeHist(src, dst);

  // show both images
  const char* source_window = "Source image";
  const char* equalized_window = "Equalized Image";

  cv::namedWindow(source_window, CV_WINDOW_AUTOSIZE);
  cv::namedWindow(equalized_window, CV_WINDOW_AUTOSIZE);

  cv::imshow(source_window, src);
  cv::imshow(equalized_window, dst);

  cv::waitKey(0);
  return 0;
}
