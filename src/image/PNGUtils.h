#ifndef _PNGUTILS_H
#define _PNGUTILS_H

#include <stdint.h>
#include <png.h>
#include <vector>

typedef struct {
  std::vector<int8_t> data;
  int width;
  int height;
  int channels;
  png_byte color_type;
  png_byte bit_depth;
} PNGImage;

typedef struct {
  unsigned char* buf;
  size_t size;
  size_t offset;
} buf_stream_info;

void ConvertRGBToRGBA(PNGImage* image);
void VertFlip(PNGImage* image);
PNGImage read_png_from_memory(const png_bytep buf, png_size_t size);
PNGImage read_png_from_file(const char* filename);
PNGImage read_png_file(const char* filename);
void write_png_file(const char* filename, char* data, int w, int h,
  int channels, int depth);

#endif  // _PNGUTILS_H
