// Copyright: This program is released into the public domain.

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

int main() {
  // Create black empty images
  cv::Mat image = cv::Mat::zeros(400, 400, CV_8UC3);

  // Draw a ellipse
  cv::ellipse(image, cv::Point(200, 200), cv::Size(100.0, 160.0),
              45, 0, 360, cv::Scalar(255, 0, 0), 1, 8);
  cv::ellipse(image, cv::Point(200, 200), cv::Size(100.0, 160.0),
              135, 0, 360, cv::Scalar(255, 0, 0), 10, 8);
  cv::ellipse(image, cv::Point(200, 200), cv::Size(150.0, 50.0),
              135, 0, 360, cv::Scalar(0, 255, 0), 1, 8);
  cv::imshow("Image", image);

  cv::waitKey(0);
  return(0);
}
