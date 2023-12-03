#ifndef BITMAP_H
#define BITMAP_H

#include "shared.h"
#include <cstddef>

class bitmap {
  size_t size;
  u8 *bit_map;

public:
  bitmap(size_t count);
  bitmap(void *buffer, u32 count);

  void set_bit(size_t position, u8 flag);
  bool get_bit(size_t position);
  u8 *get_bitmap();
  size_t get_size();
  size_t search_free();
  void print();
};

#endif // !BITMAP_H
