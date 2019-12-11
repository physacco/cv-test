// Copyright: This program is released into the public domain.

// Convert a png image file to PPM format.
// Refer to:
// https://en.wikipedia.org/wiki/Netpbm_format

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <vector>
#include <iostream>
#include "ppm.h"

typedef struct {
  std::vector<int8_t> data;
  int width;
  int height;
  int channels;
  png_byte color_type;
  png_byte bit_depth;
} PNGImage;

static const char* program = "png2ppm";
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

PNGImage read_png(const char* filename) {
  FILE* fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Error: cannot open %s: %s\n", filename, strerror(errno));
    exit(1);
  }

  unsigned char header[8];  // 8 is the maximum size that can be checked
  fread(header, 1, 8, fp);
  if (png_sig_cmp(header, 0, 8)) {
    fprintf(stderr, "Error: not recognized as a PNG file\n");
    fclose(fp);
    exit(1);
  }

  // initialize stuff
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
    nullptr, nullptr, nullptr);
  if (!png_ptr) {
    fprintf(stderr, "Error: png_create_read_struct failed\n");
    fclose(fp);
    exit(1);
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    fprintf(stderr, "Error: png_create_info_struct failed\n");
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    fclose(fp);
    exit(1);
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    fprintf(stderr, "Error: error during init_io\n");
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    fclose(fp);
    exit(1);
  }

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);

  png_read_info(png_ptr, info_ptr);

  PNGImage image;

  image.width = png_get_image_width(png_ptr, info_ptr);
  image.height = png_get_image_height(png_ptr, info_ptr);
  image.color_type = png_get_color_type(png_ptr, info_ptr);
  image.bit_depth = png_get_bit_depth(png_ptr, info_ptr);

  if (image.color_type == PNG_COLOR_TYPE_RGB) {
    image.channels = 3;
  } else if (image.color_type == PNG_COLOR_TYPE_RGBA) {
    image.channels = 4;
  } else {
    fprintf(stderr, "Error: unsupported color type: %d\n", image.color_type);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    fclose(fp);
    exit(1);
  }

  png_read_update_info(png_ptr, info_ptr);

  // read file
  if (setjmp(png_jmpbuf(png_ptr))) {
    fprintf(stderr, "Error: error during read_image\n");
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    fclose(fp);
    exit(1);
  }

  // prepare row pointers
  std::vector<png_bytep> row_pointers;
  const int chan_bytes = image.bit_depth / 8;
  const int pixel_bytes = chan_bytes * image.channels;
  const int row_bytes = pixel_bytes * image.width;
  const int mat_bytes = row_bytes * image.height;
  image.data.resize(mat_bytes);
  int8_t* data = image.data.data();
  for (int i = 0; i < image.height; ++i) {
    png_bytep row_ptr = reinterpret_cast<png_bytep>(data + row_bytes * i);
    row_pointers.push_back(row_ptr);
  }

  png_read_image(png_ptr, row_pointers.data());
  fclose(fp);
  return image;
}

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

  PNGImage image = read_png(input_file);

  FILE* fout = fopen(output_file, "wb");
  if (fout == nullptr) {
    fprintf(stderr, "Error: cannot open file %s: %s\n", output_file,
            strerror(errno));
    exit(3);
  }

  write_ppm(fout, (uint8_t*)image.data.data(), image.width, image.height, headless, binary);
  fclose(fout);

  return 0;
}
