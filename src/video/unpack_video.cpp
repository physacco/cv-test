// Copyright: This program is released into the public domain.

#include <stdio.h>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: unpack_video video_file image_dir\n");
    return 1;
  }

  const char* video_file = argv[1];
  const char* image_dir = argv[2];

  if (strlen(image_dir) > 1000) {
    fprintf(stderr, "Error: image_dir is too long\n");
    return 1;
  }

  cv::VideoCapture cap(video_file);  // open the video file
  if (!cap.isOpened()) {  // check if we succeeded
    fprintf(stderr, "Error: failed to open video file\n");
    return 2;
  }

  for (unsigned int i = 0; ; ++i) {
    cv::Mat frame;
    cap >> frame;  // read a new frame
    if (frame.empty()) {
        break;
    }

    char image_path[1024];
    snprintf(image_path, sizeof(image_path),
             "%s/%08u.jpg", image_dir, i);
    bool ok = cv::imwrite(image_path, frame);
    if (ok) {
        printf("%s\n", image_path);
        fflush(stdout);
    } else {
        fprintf(stderr, "Error: failed to write %s\n", image_path);
        return 3;
    }
  }

  cap.release();
  return 0;
}
