// Copyright: This program is released into the public domain.
//
// Convert cv::Mat data file to PNG file.
// Install dependencies:
//   apt-get install libpng-dev
// Linker flags:
//   dynamic: -L/usr/lib/x86_64-linux-gnu -lpng
//   static: /usr/lib/x86_64-linux-gnu/libpng.a

#include <errno.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

extern "C" {
#define PNG_DEBUG 3
#include <png.h>
}

#include <iostream>
#include <string>
#include <vector>

#include "PNGUtils.h"

// custom types
enum class DataFormat { RGB, BGR };

typedef struct {
  int width;
  int height;
  int quality;
  int channels;
  int depth;
  int data_offset;
  DataFormat data_format;
} Config;

static const char* program = "mat2png";
static const char* version = "0.1.0";
static const char* usage =
    "Usage: %s [options] input_file output_file\n"
    "\n"
    "Options:\n"
    "  -h, --help                   Print this help message and exit\n"
    "  -v, --version                Print version message and exit\n"
    "  -W, --width <Number>         Image width in pixels (Def: %d)\n"
    "  -H, --height <Number>        Image height in pixels (Def: %d)\n"
    "  -Q, --quality <Number>       JPEG compression quality (Def: %d).\n"
    "      --channels <Number>      RGB (3) or RGBA(4) (Def: 3)\n"
    "      --depth <Number>         Channel bit depth (Def: %d)\n"
    "      --offset <Number>        Image data offset (Def: %d)\n"
    "  -F, --format <String>        Image data format (Def: %s)\n"
    "\n";

static Config config = {
    1280,             // width
    720,              // height
    90,               // quality
    3,                // channels
    8,                // depth
    0,                // data_offset
    DataFormat::RGB,  // data_format
};

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

void write_file(const char* file_name, const char* data, size_t size) {
  FILE* f = fopen(file_name, "wb");
  if (!f) {
    fprintf(stderr, "Cannot open %s: %s\n", file_name, strerror(errno));
    exit(EXIT_FAILURE);
  }

  size_t bytes_written = fwrite(data, 1, size, f);
  if (bytes_written != size) {
    fprintf(stderr, "incomplete write: %lu/%lu, error: %s\n", size, bytes_written,
            strerror(errno));
    exit(EXIT_FAILURE);
  }

  int status = fclose(f);
  if (status != 0) {
    fprintf(stderr, "Error closing '%s': %s.\n", file_name, strerror(errno));
    exit(EXIT_FAILURE);
  }
}

void bgr2rgb(char* data, int w, int h) {
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      int offset = (i * w + j) * 3;
      char* r = &data[offset + 0];
      char* b = &data[offset + 2];
      char tmp = *r;
      *r = *b;
      *b = tmp;
    }
  }
}

void show_usage(FILE* stream) {
  const char* data_format;
  if (config.data_format == DataFormat::RGB) {
    data_format = "rgb";
  } else if (config.data_format == DataFormat::BGR) {
    data_format = "bgr";
  }

  fprintf(stream, usage, program, config.width, config.height, config.quality,
          config.data_offset, data_format);
}

int main(int argc, char** argv) {
  int show_help = 0;
  int show_version = 0;

  static struct option long_options[] = {
      {"help", no_argument, &show_help, 'h'},
      {"version", no_argument, &show_version, 'v'},
      {"width", required_argument, 0, 'W'},
      {"height", required_argument, 0, 'H'},
      {"channels", required_argument, 0, 0},
      {"depth", required_argument, 0, 0},
      {"offset", required_argument, 0, 0},
      {"format", required_argument, 0, 'F'},
      {0, 0, 0, 0}};

  while (true) {
    int opt_index = 0;
    int opt = getopt_long(argc, argv, "hvW:H:Q:F:", long_options, &opt_index);
    if (opt == -1) {
      break;
    } else if (opt == 0) {
      const char* name = long_options[opt_index].name;
      if (name != nullptr) {
        if (strcmp(name, "channels") == 0) {
          int chan = atoi(optarg);
          if (!(chan == 3 || chan == 4)) {
            fprintf(stderr, "Error: invalid value for channels\n");
            exit(EXIT_FAILURE);
          }

          config.channels = chan;
        } else if (strcmp(name, "depth") == 0) {
          int depth = atoi(optarg);
          if (!(depth == 8 || depth == 16)) {
            fprintf(stderr, "Error: invalid value for depth\n");
            exit(EXIT_FAILURE);
          }

          config.depth = depth;
        } else if (strcmp(name, "offset") == 0) {
          config.data_offset = atoi(optarg);
        } else {
          fprintf(stderr, "Error: unknown option: %s\n", name);
          exit(EXIT_FAILURE);
        }
      }
    } else if (opt == 'h') {
      show_usage(stdout);
      exit(EXIT_SUCCESS);
    } else if (opt == 'v') {
      printf("%s version %s\n", program, version);
      exit(EXIT_SUCCESS);
    } else if (opt == 'W') {
      config.width = atoi(optarg);
    } else if (opt == 'H') {
      config.height = atoi(optarg);
    } else if (opt == 'Q') {
      config.quality = atoi(optarg);
    } else if (opt == 'F') {
      if (strcmp(optarg, "rgb") == 0) {
        config.data_format = DataFormat::RGB;
      } else if (strcmp(optarg, "bgr") == 0) {
        config.data_format = DataFormat::BGR;
      } else {
        fprintf(stderr, "Unknown data format: %s\n", optarg);
        exit(EXIT_FAILURE);
      }
    } else {  // 'h'
      show_usage(stderr);
      exit(EXIT_FAILURE);
    }
  }

  if (argc - optind != 2) {
    show_usage(stderr);
    exit(EXIT_FAILURE);
  }

  const char* input_file = argv[optind++];
  const char* output_file = argv[optind++];

  std::vector<uint8_t> file_data = read_file(input_file);
  char* data = reinterpret_cast<char*>(file_data.data()) + config.data_offset;

  if (config.data_format == DataFormat::BGR) {
    bgr2rgb(data, config.width, config.height);
  }

  write_png_file(output_file, data, config.width, config.height,
    config.channels, config.depth);

  return 0;
}
