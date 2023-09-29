#include "../includes/bitmap.h"

void Bitmap::set_bit(size_t position, byte flag) {
  size_t element = position / 8;
  if (element > this->size)
    return;
  size_t offset = position % 8;

  if (flag == 1) {
    this->bitmap[element] |= (1 << offset);
  } else {
    this->bitmap[element] &= ~(1 << offset);
  }
}

byte Bitmap::get_bit(size_t position) {
  size_t element = position / 8;
  size_t offset = position % 8;

  return ((this->bitmap[element] & (1 << offset)) == (1 << offset));
}
