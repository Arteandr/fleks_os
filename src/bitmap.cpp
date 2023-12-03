#include "../includes/bitmap.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

bitmap::bitmap(size_t count) {
  this->bit_map = new u8[count / 8];
  this->size = count / 8;
}

bitmap::bitmap(void *buffer, u32 count) {
  this->bit_map = new u8[count / 8];
  memcpy(this->bit_map, buffer, count / 8 * sizeof(u8));
  // for (size_t i = 0; i < count / 8; ++i)
  //   this->bit_map[i] = (u8)buffer[i];
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

bool bitmap::get_bit(size_t position) {
  size_t element = position / 8;
  size_t offset = position % 8;

  return ((this->bit_map[element] & (1 << offset)) == (1 << offset));
}

u8 *bitmap::get_bitmap() { return this->bit_map; }

size_t bitmap::get_size() { return this->size; }

void bitmap::print() {
  std::cout << "Битовая карта: " << std::endl;
  bool before_first_empty = true;
  for (size_t i = 0; i < (this->size * 8) / 2; ++i) {
    // std::cout << (this->get_bit(i) ? "1" : "0") << " ";
    bool result = this->get_bit(i);
    if (result) {
      std::cout << "1"
                << " ";
    } else {
      if (before_first_empty) {
        std::cout << "\x1B[31m0\033[0m"
                  << " ";
        before_first_empty = false;
      } else
        std::cout << "0"
                  << " ";
    }

    if ((i + 1) % 70 == 0)
      std::cout << std::endl;
  }
  std::cout << std::endl;
}

size_t bitmap::search_free() {
  size_t counter = 0;
  bool found = false;

  for (size_t i = 0; i < get_size() && !found; ++i) {
    if (bit_map[i] == static_cast<u8>(255)) {
      counter += 8;
      continue;
    }

    for (size_t j = 0; j < 8 && !found; ++j) {
      char bitmask = (1 << j);
      if ((bit_map[i] & bitmask) == 0) {
        found = true;
      } else {
        ++counter;
      }
    }
  }

  return found ? counter : -1;
}

// size_t bitmap::search_free() {
//   size_t counter = 0;
//   bool found = false;
//
//   for (size_t i = 0; i < this->get_size(), !found; ++i) {
//     if (this->bit_map[i] == (u8)(~this->bit_map[i] | this->bit_map[i])) {
//       counter += 8;
//       continue;
//     }
//
//     for (size_t j = 0; j < 8, !found; ++j) {
//       u16 bitmask = (1 << j);
//       if ((this->bit_map[i] & bitmask) == bitmask)
//         counter++;
//       else
//         found = true;
//     }
//   }
//
//   return found ? counter : -1;
// }
