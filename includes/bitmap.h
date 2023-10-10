#ifndef BITMAP_H
#define BITMAP_H

#include "shared.h"
#include <cstddef>
#include <cstdint>
class bitmap {
  byte *bitmap;
  size_t size;

public:
  void set_bit(size_t position, byte flag);
  byte get_bit(size_t position);
};

#endif // !BITMAP_H
