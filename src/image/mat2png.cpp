// Copyright: This program is released into the public domain.
//
// Convert cv::Mat data file to JPEG file.
// Install dependencies:
//   apt-get install libjpeg8-dev
// Linker flags:
//   dynamic: -L/usr/lib/x86_64-linux-gnu -ljpeg
//   static: /usr/lib/x86_64-linux-gnu/libjpeg.a

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

// custom types
enum class DataFormat { RGB, BGR };

typedef struct {
  int width;
  int height;
  int quality;
  int data_offset;
  DataFormat data_format;
} Config;

static const png_byte ColorType = PNG_COLOR_TYPE_RGB;
static const png_byte BitDepth = 8;

static const char* program = "mat2jpg";
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
    "      --offset <Number>        Image data offset (Def: %d)\n"
    "  -F, --format <String>        Image data format (Def: %s)\n"
    "\n";

static Config config = {
    1280,             // width
    720,              // height
    90,               // quality
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

/*
void write_jpeg_file(const char* filename, char* data, int w, int h) {
  JSAMPLE* image_buffer = reinterpret_cast<JSAMPLE*>(data);
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE* outfile;            // target file
  JSAMPROW row_pointer[1];  // pointer to JSAMPLE row[s]
  int row_stride = w * 3;   // JSAMPLEs per row in image_buffer

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  if ((outfile = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "Cannot open %s: %s\n", filename, strerror(errno));
    exit(1);
  }

  jpeg_stdio_dest(&cinfo, outfile);

  cinfo.image_width = w;  // image width and height, in pixels
  cinfo.image_height = h;
  cinfo.input_components = 3;      // # of color components per pixel
  cinfo.in_color_space = JCS_RGB;  // colorspace of input image

  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, config.quality,
                   TRUE);  // limit to baseline-JPEG values

  jpeg_start_compress(&cinfo, TRUE);

  while (cinfo.next_scanline < cinfo.image_height) {
    // jpeg_write_scanlines expects an array of pointers to scanlines.
    // Here the array is only one element long, but you could pass
    // more than one scanline at a time if that's more convenient.
    row_pointer[0] = &image_buffer[cinfo.next_scanline * row_stride];
    (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  // Finish compression
  jpeg_finish_compress(&cinfo);
  fclose(outfile);

  // release JPEG compression object
  // This is an important step since it will release a good deal of memory.
  jpeg_destroy_compress(&cinfo);
}

void write_jpeg_file2(const char* filename, char* data, int w, int h) {
  JSAMPLE* image_buffer = reinterpret_cast<JSAMPLE*>(data);
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPROW row_pointer[1];  // pointer to JSAMPLE row[s]
  int row_stride = w * 3;   // JSAMPLEs per row in image_buffer

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  uint8_t* outbuffer = nullptr;  // must be 0
  size_t outsize = 0;  // must be 0
  jpeg_mem_dest(&cinfo, &outbuffer, &outsize);

  cinfo.image_width = w;  // image width and height, in pixels
  cinfo.image_height = h;
  cinfo.input_components = 3;      // # of color components per pixel
  cinfo.in_color_space = JCS_RGB;  // colorspace of input image

  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, config.quality,
                   TRUE);  // limit to baseline-JPEG values

  jpeg_start_compress(&cinfo, TRUE);

  while (cinfo.next_scanline < cinfo.image_height) {
    // jpeg_write_scanlines expects an array of pointers to scanlines.
    // Here the array is only one element long, but you could pass
    // more than one scanline at a time if that's more convenient.
    row_pointer[0] = &image_buffer[cinfo.next_scanline * row_stride];
    (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  // Finish compression
  jpeg_finish_compress(&cinfo);

  // release JPEG compression object
  // This is an important step since it will release a good deal of memory.
  jpeg_destroy_compress(&cinfo);

  // Write file
  write_file(filename, (const char*)outbuffer, outsize);

  // free the jpeg buffer
  free(outbuffer);
}
*/

void write_png_file(const char* filename, char* data, int w, int h) {
  FILE* fp = fopen(filename, "wb");
  if (fp == nullptr) {
    fprintf(stderr, "Cannot open %s: %s\n", filename, strerror(errno));
    exit(1);
  }

  // initialize stuff
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) {
    fprintf(stderr, "Error: png_create_write_struct failed\n");
    fclose(fp);
    exit(1);
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    fprintf(stderr, "Error: png_create_info_struct failed\n");
    png_destroy_write_struct(&png_ptr, (png_infopp)nullptr);
    fclose(fp);
    exit(1);
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    fprintf(stderr, "Error: error during init_io\n");
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    exit(1);
  }

  png_init_io(png_ptr, fp);

  // write header
  if (setjmp(png_jmpbuf(png_ptr))) {
    fprintf(stderr, "Error: error during writing header\n");
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    exit(1);
  }

  png_set_IHDR(png_ptr, info_ptr, w, h,
               BitDepth, ColorType, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  // prepare row pointers
  std::vector<png_bytep> row_pointers;
  const int chan_width = BitDepth / 8;
  const int row_bytes = chan_width * 3 * w;
  for (int i = 0; i < h; ++i) {
    png_bytep row_ptr = reinterpret_cast<png_bytep>(data + row_bytes * i);
    row_pointers.push_back(row_ptr);
  }

  // write bytes
  if (setjmp(png_jmpbuf(png_ptr))) {
    fprintf(stderr, "Error: error during writing body\n");
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    exit(1);
  }

  png_write_image(png_ptr, row_pointers.data());

  // end write
  if (setjmp(png_jmpbuf(png_ptr))) {
    fprintf(stderr, "Error: error during end of write\n");
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    exit(1);
  }

  png_write_end(png_ptr, NULL);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
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
        if (strcmp(name, "offset") == 0) {
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

  // write_jpeg_file2(output_file, data, config.width, config.height);
  write_png_file(output_file, data, config.width, config.height);

  return 0;
}
