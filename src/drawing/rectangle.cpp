// Copyright: This program is released into the public domain.

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

int main() {
  // Create black empty images
  cv::Mat image = cv::Mat::zeros(400, 400, CV_8UC3);

  // Draw a rectangle ( 5th argument is not -ve)
  cv::rectangle(image, cv::Point(15, 20), cv::Point(70, 50),
                cv::Scalar(0, 55, 255), +1, 4);
  cv::imshow("Image1", image);

  // Draw a filled rectangle ( 5th argument is -ve)
  cv::rectangle(image, cv::Point(115, 120), cv::Point(170, 150),
                cv::Scalar(100, 155, 25), -1, 8);
  cv::imshow("Image2", image);

  cv::waitKey(0);
  return(0);
}
