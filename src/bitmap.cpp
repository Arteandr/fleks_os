#include "../includes/bitmap.h"

bitmap::bitmap(size_t count) {
  this->bit_map = new u8[count / 8];
  this->set_bit(0, true);
  this->size = count / 8;
}

void bitmap::set_bit(size_t position, u8 flag) {
  size_t element = position / 8;
  if (element > this->size)
    return;
  size_t offset = position % 8;

  if (flag == 1) {
    this->bit_map[element] |= (1 << offset);
  } else {
    this->bit_map[element] &= ~(1 << offset);
  }
}

u8 bitmap::get_bit(size_t position) {
  size_t element = position / 8;
  size_t offset = position % 8;

  return ((this->bit_map[element] & (1 << offset)) == (1 << offset));
}

u8 *bitmap::get_bitmap() { return this->bit_map; }

size_t bitmap::get_size() { return this->size; }
