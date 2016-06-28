// Copyright: This program is released into the public domain.

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

int main() {
  // Create black empty images
  cv::Mat image = cv::Mat::zeros(400, 400, CV_8UC3);

  // Draw a circle
  cv::circle(image, cv::Point(200, 200), 32.0, cv::Scalar(0, 0, 255), 1, 8);
  cv::imshow("Image", image);

  cv::waitKey(0);
  return(0);
}
