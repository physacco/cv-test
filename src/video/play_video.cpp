// Copyright: This program is released into the public domain.

#include <stdio.h>
#include <string>
#include <opencv2/opencv.hpp>

std::string fourcc_to_string(uint32_t fourcc) {
  char buf[5] = {0, 0, 0, 0, 0};
  buf[0] = static_cast<char>(fourcc & 0xff);
  buf[1] = static_cast<char>((fourcc >> 8) & 0xff);
  buf[2] = static_cast<char>((fourcc >> 16) & 0xff);
  buf[3] = static_cast<char>((fourcc >> 24) & 0xff);
  return buf;
}

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

  int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
  int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
  double fps = cap.get(cv::CAP_PROP_FPS);
  int frames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
  uint32_t fourcc = static_cast<uint32_t>(cap.get(cv::CAP_PROP_FOURCC));

  printf("Width: %d\n", width);
  printf("Height: %d\n", height);
  printf("FPS: %.2f\n", fps);
  printf("Frames: %d\n", frames);
  printf("Codec: %s\n", fourcc_to_string(fourcc).c_str());

  cv::namedWindow("Video", 1);
  while (true) {
    cv::Mat frame;
    cap >> frame;  // read a new frame
    if (frame.empty()) {  // see [1]
      break;
    }

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

// [1] The result of cv::imread, cv::VideoCapture::read
// can be determined by the output Mat's properties:
// Success: cols >  0, rows >  0, data != NULL, empty() == 0
// Failure: cols == 0, rows == 0, data == NULL, empty() == 1
