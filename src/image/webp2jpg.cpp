// Copyright: This program is released into the public domain.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>

const char* find_ext_name(const char* str) {
    if (str == NULL) {
        return NULL;
    }

    size_t len = strlen(str);
    const char* end = str + len;

    while (true) {
        --end;
        if (*end == '.' || end == str) {
            break;
        }
    }

    return end;
}

int compare_ext_name(const char* str, const char* ext_name) {
    const char* str_ext_name = find_ext_name(str);
    return strcmp(str_ext_name, ext_name);
}

const char* input_ext_name = ".webp";
const char* output_ext_name = ".jpg";

int main(int argc, char** argv) {
  if (argc != 4) {
    std::cerr << "Usage: webp2jpg input_file output_file quality\n";
    exit(1);
  }

  const char* input_file = argv[1];
  if (compare_ext_name(input_file, input_ext_name) != 0) {
    std::cerr << "Error: input file extension must be "
              << input_ext_name << std::endl;
    exit(1);
  }

  const char* output_file = argv[2];
  if (compare_ext_name(output_file, ".jpg") != 0) {
    std::cerr << "Error: output file extension must be "
              << output_ext_name << std::endl;
    exit(1);
  }

  int quality = atoi(argv[3]);
  if (quality <= 0) {
    std::cerr << "Error: quality must be between [1, 100]\n";
    exit(1);
  }

  cv::Mat image = cv::imread(input_file, cv::IMREAD_COLOR);
  if (image.empty()) {
    std::cout <<  "Could not open or find the image" << std::endl;
    exit(2);
  }

  std::vector<int> compression_params;
  compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
  compression_params.push_back(quality);

  try {
    fprintf(stderr, "Saving image file ...\n");
    int ok = cv::imwrite(output_file, image, compression_params);
    if (ok) {
      printf("Image saved to [%s]\n", output_file);
    } else {
      fprintf(stderr, "Cannot save image: %s\n", output_file);
      exit(3);
    }
  } catch (std::runtime_error& ex) {
    fprintf(stderr, "Cannot save image: %s\n", ex.what());
    exit(3);
  }

  return 0;
}
