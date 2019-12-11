
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "PNGUtils.h"

void ConvertRGBToRGBA(PNGImage* image) {
  int w = image->width;
  int h = image->height;
  int c = image->channels;

  if (c != 3) {
    return;
  }

  std::vector<int8_t> new_data(w * h * 4);

  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      int offset = (i * w + j) * 3;
      int r = image->data[offset + 0];
      int g = image->data[offset + 1];
      int b = image->data[offset + 2];

      int new_offset = (i * w + j) * 4;
      new_data[new_offset + 0] = r;
      new_data[new_offset + 1] = g;
      new_data[new_offset + 2] = b;
      new_data[new_offset + 3] = (int8_t)255;
    }
  }

  image->data = new_data;
}

void VertFlip(PNGImage* image) {
  int w = image->width;
  int h = image->height;
  int c = image->channels;

  std::vector<int8_t> new_data(w * h * 4);

  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      int offset = (i * w + j) * c;
      int r = image->data[offset + 0];
      int g = image->data[offset + 1];
      int b = image->data[offset + 2];
      int a = 255;
      if (c == 4) {
        a = image->data[offset + 3];
      }

      int new_i = h - i - 1;  // reverse scanline order
      int new_offset = (new_i * w + j) * 4;
      new_data[new_offset + 0] = r;
      new_data[new_offset + 1] = g;
      new_data[new_offset + 2] = b;
      new_data[new_offset + 3] = a;
    }
  }

  image->data = new_data;
}

static void buf_stream_read(png_structp png, png_bytep buf, png_size_t size) {
  buf_stream_info* stream = (buf_stream_info*)png_get_io_ptr(png);
  if (!stream) {
    return;
  }

  size_t left = stream->size - stream->offset;
  if (size > left) {
    fprintf(stderr, "Error: cannot read %zu bytes (only %zu)\n", size, left);
    return;
  }

  unsigned char* ptr = stream->buf + stream->offset;
  memcpy(buf, ptr, size);
  stream->offset += size;
}

PNGImage read_png_from_memory(const png_bytep buf, png_size_t size) {
  // initialize stuff
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
    nullptr, nullptr, nullptr);
  if (!png_ptr) {
    fprintf(stderr, "Error: png_create_read_struct failed\n");
    exit(1);
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    fprintf(stderr, "Error: png_create_info_struct failed\n");
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    exit(1);
  }

  // if some error occurs, it will jump here
  if (setjmp(png_jmpbuf(png_ptr))) {
    fprintf(stderr, "Error: error during init_io\n");
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    exit(1);
  }

  // set custom data reader
  buf_stream_info stream {buf, size, 0};
  png_set_read_fn(png_ptr, &stream, buf_stream_read);

  // read png header
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
    exit(1);
  }

  png_read_update_info(png_ptr, info_ptr);

  // read file
  if (setjmp(png_jmpbuf(png_ptr))) {
    fprintf(stderr, "Error: error during read_image\n");
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
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
  return image;
}

static std::vector<char> read_file(const char* filename) {
  std::vector<char> data;

  FILE* fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Error: cannot open %s: %s\n", filename, strerror(errno));
    exit(1);
  }

  int ret = fseek(fp, 0, SEEK_END);
  if (ret == -1) {
    fprintf(stderr, "Error: fseek error: %s\n", strerror(errno));
    exit(1);
  }

  long file_size = ftell(fp);
  if (file_size == -1) {
    fprintf(stderr, "Error: ftell error: %s\n", strerror(errno));
    exit(1);
  }

  ret = fseek(fp, 0, SEEK_SET);
  if (ret == -1) {
    fprintf(stderr, "Error: fseek error: %s\n", strerror(errno));
    exit(1);
  }

  data.resize(file_size);

  size_t bytes_read = fread(data.data(), 1, file_size, fp);
  if (bytes_read != (size_t)file_size) {
    fprintf(stderr, "Error: failed to read file: %s\n", filename);
    exit(1);
  }

  fclose(fp);
  return data;
}

PNGImage read_png_from_file(const char* filename) {
  std::vector<char> data = read_file(filename);
  return read_png_from_memory((const png_bytep)data.data(), data.size());
}

PNGImage read_png_file(const char* filename) {
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

void write_png_file(const char* filename, char* data, int w, int h,
  int channels, int depth) {
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

  png_byte bit_depth = depth;

  png_byte color_type;
  if (channels == 3) {
    color_type = PNG_COLOR_TYPE_RGB;
  } else {
    color_type = PNG_COLOR_TYPE_RGBA;
  }

  png_set_IHDR(png_ptr, info_ptr, w, h,
               bit_depth, color_type, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  // prepare row pointers
  std::vector<png_bytep> row_pointers;
  const int chan_bytes = bit_depth / 8;
  const int pixel_bytes = chan_bytes * channels;
  const int row_bytes = pixel_bytes * w;
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
