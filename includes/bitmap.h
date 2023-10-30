#ifndef BITMAP_H
#define BITMAP_H

#include "shared.h"
#include <cstddef>
#include <cstdint>
class bitmap {
  size_t size;
  u8 *bit_map;

public:
  bitmap(size_t count);
  void set_bit(size_t position, u8 flag);
  u8 get_bit(size_t position);
  u8 *get_bitmap();
  size_t get_size();
  size_t search_free();
};

#endif // !BITMAP_H
