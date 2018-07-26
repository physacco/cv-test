// Copyright: This program is released into the public domain.

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

static const char* program = "show_raw_mat";
static const char* version = "0.1.0";
static const char* usage =
    "Usage: %s [options] file1 [file2 ...]\n"
    "\n"
    "Options:\n"
    "  -h, --help                   Print this help message and exit\n"
    "  -v, --version                Print version message and exit\n"
    "  -W, --width <Number>         Image width in pixels (Def: %d)\n"
    "  -H, --height <Number>        Image height in pixels (Def: %d)\n"
    "\n";

static int ImageWidth = 1280;
static int ImageHeight = 720;
static std::vector<std::string> InputFiles;

size_t get_file_size(const char* file_name) {
  struct stat sb;
  if (stat(file_name, &sb) != 0) {
    fprintf(stderr, "stat failed for '%s': %s\n", file_name, strerror(errno));
    exit(EXIT_FAILURE);
  }

  return sb.st_size;
}

std::vector<uint8_t> read_file(const char* file_name) {
  size_t size = get_file_size(file_name);
  std::vector<uint8_t> data(size);

  FILE* f = fopen(file_name, "rb");
  if (!f) {
    fprintf(stderr, "Cannot open %s: %s\n", file_name, strerror(errno));
    exit(EXIT_FAILURE);
  }

  size_t bytes_read = fread(data.data(), 1, size, f);
  if (bytes_read != size) {
    fprintf(stderr, "incomplete read: %lu/%lu, error: %s\n", size, bytes_read,
            strerror(errno));
    exit(EXIT_FAILURE);
  }

  int status = fclose(f);
  if (status != 0) {
    fprintf(stderr, "Error closing '%s': %s.\n", file_name, strerror(errno));
    exit(EXIT_FAILURE);
  }

  return data;
}

void show_images() {
  cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
  for (auto& path : InputFiles) {
    fprintf(stderr, "Reading %s\n", path.c_str());
    std::vector<uint8_t> data = read_file(path.c_str());
    cv::Mat image(cv::Size(ImageWidth, ImageHeight), CV_8UC3, data.data());
    cv::imshow(path, image);
    cv::moveWindow(path, 10, 10);
    int key = cv::waitKey(0) & 0xff;
    if (key == 'q') {
      break;
    }
  }
}

int main(int argc, char** argv) {
  int show_help = 0;
  int show_version = 0;

  static struct option long_options[] = {
      {"help", no_argument, &show_help, 'h'},
      {"version", no_argument, &show_version, 'v'},
      {"width", required_argument, 0, 'W'},
      {"height", required_argument, 0, 'H'},
      {0, 0, 0, 0}};

  while (true) {
    int opt = getopt_long(argc, argv, "hvW:H:", long_options, nullptr);
    if (opt == -1) {
      break;
    } else if (opt == 'h') {
      printf(usage, program, ImageWidth, ImageHeight);
      exit(EXIT_SUCCESS);
    } else if (opt == 'v') {
      printf("%s version %s\n", program, version);
      exit(EXIT_SUCCESS);
    } else if (opt == 'W') {
      ImageWidth = atoi(optarg);
    } else if (opt == 'H') {
      ImageHeight = atoi(optarg);
    } else {  // 'h'
      fprintf(stderr, usage, program, ImageWidth, ImageHeight);
      exit(EXIT_FAILURE);
    }
  }

  for (int i = optind; i < argc; ++i) {
    InputFiles.push_back(argv[i]);
  }

  if (InputFiles.empty()) {
    fprintf(stderr, "Error: no input files\n");
    exit(EXIT_FAILURE);
  }

  show_images();
  return 0;
}
