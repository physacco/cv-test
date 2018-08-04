// Copyright: This program is released into the public domain.

// Refer to:
// https://docs.opencv.org/3.1.0/d3/d63/classcv_1_1Mat.html

#include <iostream>
#include <string>
#include <map>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#define ADD_TYPE(t) res[t] = #t;

//  A complete list of matrix types contains the following values:
//    CV_8U - 8-bit unsigned integers ( 0..255 )
//    CV_8S - 8-bit signed integers ( -128..127 )
//    CV_16U - 16-bit unsigned integers ( 0..65535 )
//    CV_16S - 16-bit signed integers ( -32768..32767 )
//    CV_32S - 32-bit signed integers ( -2147483648..2147483647 )
//    CV_32F - 32-bit floating-point numbers ( -FLT_MAX..FLT_MAX, INF, NAN )
//    CV_64F - 64-bit floating-point numbers ( -DBL_MAX..DBL_MAX, INF, NAN )
std::map<int, std::string> GetMatDepthMap() {
  std::map<int, std::string> res;

  ADD_TYPE(CV_8U);
  ADD_TYPE(CV_8S);
  ADD_TYPE(CV_16U);
  ADD_TYPE(CV_16S);
  ADD_TYPE(CV_32S);
  ADD_TYPE(CV_32F);
  ADD_TYPE(CV_64F);

  return res;
}

std::map<int, std::string> GetMatTypeMap() {
  std::map<int, std::string> res;

  ADD_TYPE(CV_8UC1);
  ADD_TYPE(CV_8UC2);
  ADD_TYPE(CV_8UC3);
  ADD_TYPE(CV_8UC4);

  ADD_TYPE(CV_8SC1);
  ADD_TYPE(CV_8SC2);
  ADD_TYPE(CV_8SC3);
  ADD_TYPE(CV_8SC4);

  ADD_TYPE(CV_16UC1);
  ADD_TYPE(CV_16UC2);
  ADD_TYPE(CV_16UC3);
  ADD_TYPE(CV_16UC4);

  ADD_TYPE(CV_16SC1);
  ADD_TYPE(CV_16SC2);
  ADD_TYPE(CV_16SC3);
  ADD_TYPE(CV_16SC4);

  ADD_TYPE(CV_32SC1);
  ADD_TYPE(CV_32SC2);
  ADD_TYPE(CV_32SC3);
  ADD_TYPE(CV_32SC4);

  ADD_TYPE(CV_32FC1);
  ADD_TYPE(CV_32FC2);
  ADD_TYPE(CV_32FC3);
  ADD_TYPE(CV_32FC4);

  ADD_TYPE(CV_64FC1);
  ADD_TYPE(CV_64FC2);
  ADD_TYPE(CV_64FC3);
  ADD_TYPE(CV_64FC4);

  return res;
}

#undef ADD_TYPE

void PrintMatProps(const cv::Mat& image) {
  printf("data: %p\n", image.data);

  printf("cols: %d\n", image.cols);
  printf("rows: %d\n", image.rows);
  printf("dims: %d\n", image.dims);

  printf("flags: 0x%08X\n", image.flags);
  printf("  magic: 0x%08X\n", image.flags & CV_MAGIC_MASK);
  printf("  submatrix: 0x%08X\n", image.flags & CV_SUBMAT_FLAG);
  printf("  continuous: 0x%08X\n", image.flags & CV_MAT_CONT_FLAG);
  printf("  type: 0x%08X\n", image.flags & CV_MAT_TYPE_MASK);

  // Returns the number of matrix channels.
  printf("channels(): %d\n", image.channels());

  //  Returns the depth of a matrix element.
  //  The method returns the identifier of the matrix element depth
  //  (the type of each individual channel). For example, for a
  //  16-bit signed element array, the method returns CV_16S .
  const char* depth = "unknown";
  auto depth_map = GetMatDepthMap();
  auto it1 = depth_map.find(image.depth());
  if (it1 != depth_map.end()) {
    depth = it1->second.c_str();
  }

  printf("depth(): %s\n", depth);

  // Returns the matrix element size in bytes.
  printf("elemSize(): %d\n", (int)image.elemSize());

  // Returns the size of each matrix element channel in bytes.
  printf("elemSize1(): %d\n", (int)image.elemSize1());

  // Reports whether the matrix is continuous or not.
  printf("isContinuous(): %d\n", (int)image.isContinuous());

  // Returns the total number of array elements.
  printf("total(): %d\n", (int)image.total());

  // Returns the type of a matrix element.
  const char* type = "unknown";
  auto type_map = GetMatTypeMap();
  auto it2 = type_map.find(image.type());
  if (it2 != type_map.end()) {
    type = it2->second.c_str();
  }

  printf("type(): %s\n", type);
}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: show_image filename\n";
    return 1;
  }

  cv::Mat image = cv::imread(argv[1], cv::IMREAD_UNCHANGED);
  if (image.empty()) {  // see [1]
    std::cout <<  "Could not open or find the image" << std::endl;
    return -1;
  }

  PrintMatProps(image);
  cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);  // Create a window
  cv::imshow("Display window", image);  // Show our image inside it.

  cv::waitKey(0);  // Wait for a keystroke in the window
  return 0;
}

// [1] The result of cv::imread, cv::VideoCapture::read
// can be determined by the output Mat's properties:
// Success: cols >  0, rows >  0, data != NULL, empty() == 0
// Failure: cols == 0, rows == 0, data == NULL, empty() == 1
