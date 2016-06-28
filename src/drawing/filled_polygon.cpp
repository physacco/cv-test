// Copyright: This program is released into the public domain.

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

int main() {
  // Create black empty images
  cv::Mat image = cv::Mat::zeros(400, 400, CV_8UC3);

  int w = 400;
  // Draw a circle
  /** Create some points */
  cv::Point rook_points[1][20];
  rook_points[0][0] = cv::Point(w/4.0, 7*w/8.0);
  rook_points[0][1] = cv::Point(3*w/4.0, 7*w/8.0);
  rook_points[0][2] = cv::Point(3*w/4.0, 13*w/16.0);
  rook_points[0][3] = cv::Point(11*w/16.0, 13*w/16.0);
  rook_points[0][4] = cv::Point(19*w/32.0, 3*w/8.0);
  rook_points[0][5] = cv::Point(3*w/4.0, 3*w/8.0);
  rook_points[0][6] = cv::Point(3*w/4.0, w/8.0);
  rook_points[0][7] = cv::Point(26*w/40.0, w/8.0);
  rook_points[0][8] = cv::Point(26*w/40.0, w/4.0);
  rook_points[0][9] = cv::Point(22*w/40.0, w/4.0);
  rook_points[0][10] = cv::Point(22*w/40.0, w/8.0);
  rook_points[0][11] = cv::Point(18*w/40.0, w/8.0);
  rook_points[0][12] = cv::Point(18*w/40.0, w/4.0);
  rook_points[0][13] = cv::Point(14*w/40.0, w/4.0);
  rook_points[0][14] = cv::Point(14*w/40.0, w/8.0);
  rook_points[0][15] = cv::Point(w/4.0, w/8.0);
  rook_points[0][16] = cv::Point(w/4.0, 3*w/8.0);
  rook_points[0][17] = cv::Point(13*w/32.0, 3*w/8.0);
  rook_points[0][18] = cv::Point(5*w/16.0, 13*w/16.0);
  rook_points[0][19] = cv::Point(w/4.0, 13*w/16.0);

  const cv::Point* ppt[1] = { rook_points[0] };
  int npt[] = { 20 };

  cv::fillPoly(image, ppt, npt, 1, cv::Scalar(255, 255, 255), 8);
  cv::imshow("Image", image);

  cv::waitKey(0);
  return(0);
}
