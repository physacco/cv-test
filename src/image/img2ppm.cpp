// Copyright: This program is released into the public domain.

// Convert an image file to PPM format.
// Refer to:
// https://en.wikipedia.org/wiki/Netpbm_format

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opencv2/opencv.hpp>
#include "ppm.h"

static const char* program = "img2ppm";
static const char* version = "0.1.0";
static const char* usage =
    "Usage: %s [options] input_file output_file\n"
    "\n"
    "Options:\n"
    "  -h, --help                   Print this help message and exit\n"
    "  -v, --version                Print version message and exit\n"
    "  -A, --ascii                  Use the ASCII (P3) format\n"
    "  -H, --headless               Do not write the file header\n"
    "\n";

static bool binary = true;
static bool headless = false;

int main(int argc, char** argv) {
  int show_help = 0;
  int show_version = 0;

  static struct option long_options[] = {
      {"help", no_argument, &show_help, 'h'},
      {"version", no_argument, &show_version, 'v'},
      {"ascii", no_argument, 0, 'A'},
      {0, 0, 0, 0}};

  while (true) {
    int opt = getopt_long(argc, argv, "hvAH", long_options, nullptr);
    if (opt == -1) {
      break;
    } else if (opt == 'h') {
      printf(usage, program);
      exit(EXIT_SUCCESS);
    } else if (opt == 'v') {
      printf("%s version %s\n", program, version);
      exit(EXIT_SUCCESS);
    } else if (opt == 'A') {
      binary = false;
    } else if (opt == 'H') {
      headless = true;
    } else {  // 'h'
      fprintf(stderr, usage, program);
      exit(EXIT_FAILURE);
    }
  }

  if (argc - optind != 2) {
    fprintf(stderr, usage, program);
    exit(EXIT_FAILURE);
  }

  const char* input_file = argv[optind++];
  const char* output_file = argv[optind++];

  cv::Mat bgr_image = cv::imread(input_file, cv::IMREAD_COLOR);
  if (bgr_image.empty()) {  // see [1]
    fprintf(stderr, "Error: cannot open file %s\n", input_file);
    exit(2);
  }

  cv::Mat rgb_image;
  cv::cvtColor(bgr_image, rgb_image, cv::COLOR_BGR2RGB);
  int width = rgb_image.cols;
  int height = rgb_image.rows;

  FILE* fout = fopen(output_file, "wb");
  if (fout == nullptr) {
    fprintf(stderr, "Error: cannot open file %s: %s\n", output_file,
            strerror(errno));
    exit(3);
  }

  write_ppm(fout, (uint8_t*)rgb_image.data, width, height, headless, binary);
  fclose(fout);

  return 0;
}
