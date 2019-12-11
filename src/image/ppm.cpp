#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "ppm.h"

int write_ppm_data_ascii(FILE* f, uint8_t* data, int w, int h) {
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      int offset = (i * w + j) * 3;
      int r = data[offset + 0];
      int g = data[offset + 1];
      int b = data[offset + 2];
      int ret = fprintf(f, "%d %d %d\n", r, g, b);
      if (ret < 0) {
        fprintf(stderr, "Error: write error: %s\n", strerror(errno));
        return 1;
      }
    }
  }

  return 0;
}

int write_ppm_data_binary(FILE* f, uint8_t* data, int w, int h) {
  int data_size = w * h * 3;
  int bytes = fwrite(data, 1, data_size, f);
  if (bytes < data_size) {
    fprintf(stderr, "Error: incomplete write: %d/%d: %s\n", bytes, data_size,
            strerror(errno));
    return 1;
  }

  return 0;
}

int write_ppm(FILE* f, uint8_t* data, int w, int h, bool headless, bool binary) {
  if (headless == false) {
    if (binary) {
      fprintf(f, "P6\n");
    } else {
      fprintf(f, "P3\n");
    }

    fprintf(f, "%d %d\n", w, h);
    fprintf(f, "%d\n", 255);  // max color
  }

  if (binary) {
    return write_ppm_data_binary(f, data, w, h);
  } else {
    return write_ppm_data_ascii(f, data, w, h);
  }
}
