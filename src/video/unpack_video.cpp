// Copyright: This program is released into the public domain.

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>
#include <vector>
#include <opencv2/opencv.hpp>

static const char* program = "unpack_video";
static const char* version = "0.1.0";
static const char* usage = "Usage: %s [options] video_file image_dir\n"
"\n"
"Options:\n"
"  -h, --help                   Print this help message and exit\n"
"  -v, --version                Print version message and exit\n"
"  -Q, --quality <Number>       Set image quality (0-100, default: 95)\n"
"\n";

int parse_quality(const char* str) {
  // check quality format: [0-9]+
  for (size_t i = 0, len = strlen(str); i < len; ++i) {
    if (!isdigit(str[i])) {  // bad format
      return -1;
    }
  }

  // good format
  return atoi(str);
}

void unpack_video(const char* video_file, const char* image_dir, int quality) {
  cv::VideoCapture cap(video_file);  // open the video file
  if (!cap.isOpened()) {  // check if we succeeded
    fprintf(stderr, "Error: failed to open video file: %s\n", video_file);
    exit(2);
  }

  std::vector<int> imwrite_params = {cv::IMWRITE_JPEG_QUALITY, quality};

  for (unsigned int i = 0; ; ++i) {
    cv::Mat frame;
    cap >> frame;  // read a new frame
    if (frame.empty()) {
        break;
    }

    char image_path[1024];
    snprintf(image_path, sizeof(image_path),
             "%s/%08u.jpg", image_dir, i);

    bool ok = cv::imwrite(image_path, frame, imwrite_params);
    if (ok) {
      printf("%s\n", image_path);
      fflush(stdout);
    } else {
      fprintf(stderr, "Error: failed to write %s\n", image_path);
      exit(3);
    }
  }

  cap.release();
}

int main(int argc, char** argv) {
  int show_help = 0;
  int show_version = 0;
  int quality = 95;

  static struct option long_options[] = {
    {"help", no_argument, &show_help, 'h'},
    {"version", no_argument, &show_version, 'v'},
    {"quality", required_argument, 0, 'Q'},
    {0, 0, 0, 0}
  };

  while (true) {
    int opt = getopt_long(argc, argv, "hvQ:", long_options, nullptr);
    if (opt == -1) {
      break;
    } else if (opt == 'h') {
      printf(usage, program);
      exit(EXIT_SUCCESS);
    } else if (opt == 'v') {
      printf("%s version %s\n", program, version);
      exit(EXIT_SUCCESS);
    } else if (opt == 'Q') {
      quality = parse_quality(optarg);
      if (quality < 0 || quality > 100) {
        fprintf(stderr, "Error: invalid quality. "
                "It should be between [0, 100]\n");
        exit(EXIT_FAILURE);
      }
    } else {  // 'h'
      fprintf(stderr, usage, program);
      exit(EXIT_FAILURE);
    }
  }

  if (argc - optind != 2) {
    fprintf(stderr, usage, program);
    exit(EXIT_FAILURE);
  }

  const char* video_file = argv[optind++];
  const char* image_dir = argv[optind++];

  if (strlen(image_dir) > 1000) {
    fprintf(stderr, "Error: image_dir is too long\n");
    exit(EXIT_FAILURE);
  }

  unpack_video(video_file, image_dir, quality);

  return 0;
}
