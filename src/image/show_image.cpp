// Copyright: This program is released into the public domain.

#include <iostream>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: show_image filename\n";
    return 1;
  }

  cv::Mat image = cv::imread(argv[1], cv::IMREAD_COLOR);
  if (image.empty()) {  // see [1]
    std::cout <<  "Could not open or find the image" << std::endl;
    return -1;
  }

  cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);  // Create a window
  cv::imshow("Display window", image);  // Show our image inside it.

  cv::waitKey(0);  // Wait for a keystroke in the window
  return 0;
}

// [1] The result of cv::imread, cv::VideoCapture::read
// can be determined by the output Mat's properties:
// Success: cols >  0, rows >  0, data != NULL, empty() == 0
// Failure: cols == 0, rows == 0, data == NULL, empty() == 1
