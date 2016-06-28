// Copyright: This program is released into the public domain.

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

int main() {
  // Create black empty images
  cv::Mat image = cv::Mat::zeros(400, 400, CV_8UC3);

  // Draw a line
  cv::line(image, cv::Point(15, 20), cv::Point(70, 50),
           cv::Scalar(110, 220, 0),  2, 8);
  cv::imshow("Image", image);

  cv::waitKey(0);
  return(0);
}
