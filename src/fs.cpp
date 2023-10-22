#include "../includes/fs.h"
#include "../includes/superblock.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>

FS::FS(std::string filename) {
  this->fd.open(filename.c_str(),
                std::ios::out | std::ios::binary | std::ios::app);
  if (!this->fd.is_open()) {
    std::string error = "Ошибка при открытии файла ФС - \"" + filename + "\"";
    throw std::runtime_error(error);
  }

  // this->superblock = (super_block *)malloc(SUPER_SIZE);
  // memset(this->superblock, 0, SUPER_SIZE);
  //
  // // Устанавливаем смещение суперблока
  // this->fd.seekg(SUPER_OFFSET, std::ios::beg);
  // this->fd.read((char *)this->superblock, SUPER_SIZE);
  // if (this->fd.gcount() != SUPER_SIZE) {
  //   std::string error = "Невозможно прочитать суперблок";
  //   throw std::runtime_error(error);
  //
  // }
}

// char *align(void *s, size_t align_size) {
//   size_t struct_size = sizeof(s);
//   size_t padding = align_size - (struct_size % align_size);
//   size_t total_size = struct_size + padding;
//
//   char *buffer = (char *)malloc(total_size);
//   memset(buffer, 0, total_size);
//   memcpy(buffer, &s, struct_size);
//
//   return buffer;
// }

// size - Размер в байтах
void FS::format(size_t fs_size, size_t block_size) {
  // Высчитываем необходимое количество блоков
  u32 blocks_count = fs_size / block_size;
  // Количество inode в группе
  u32 inodes_per_group = block_size * 8;
  // Количество групп блоков
  u32 groups_count = std::ceil(blocks_count / BLOCKS_PER_GROUP);
  // Общее количество inode
  u32 inodes_count = inodes_per_group * groups_count;

  super_block *sb = new super_block;
  sb->s_inodes_count = inodes_count;
  sb->s_blocks_count = blocks_count;
  sb->s_free_indes_count = inodes_count;
  sb->s_block_size = block_size;
  sb->s_blocks_per_group = BLOCKS_PER_GROUP;
  sb->s_inodes_per_group = inodes_per_group;
  sb->s_magic = SUPER_MAGIC;

  group_desc *group_desc_table =
      (group_desc *)malloc(groups_count * sizeof(group_desc));

  std::string filename = "filesystem";
  std::fstream fd = std::fstream(filename, std::ios::out | std::ios::binary);
  if (!fd.is_open()) {
    std::string error = "Ошибка при открытии файла ФС - \"" + filename + "\"";
    throw std::runtime_error(error);
  }

  auto diff = [](size_t total, size_t remaining) { return total - remaining; };
  auto cursor = [block_size, diff](size_t group_no, size_t start_blocks,
                                   size_t remaining_blocks) {
    return ((BLOCKS_PER_GROUP * (group_no - 1)) +
            diff(start_blocks, remaining_blocks)) *
           block_size;
  };

  fd.seekg(0, std::ios::beg);
  for (size_t group_no = 1; group_no <= groups_count; group_no++) {
    size_t remaining_blocks = BLOCKS_PER_GROUP;
    // Если последняя группа не вмещается полностью получаем остаток блоков
    // для последней группы
    if (blocks_count % BLOCKS_PER_GROUP != 0 && group_no == groups_count)
      remaining_blocks = blocks_count % BLOCKS_PER_GROUP;
    const size_t start_blocks_count = remaining_blocks;

    if (group_no == 1) {
      fd.write(reinterpret_cast<char *>(&sb), sizeof(*sb));
      fd.seekg(block_size);
      fd.write(reinterpret_cast<char *>(&group_desc_table),
               sizeof(*group_desc_table));
      fd.seekg(block_size * 2);
      remaining_blocks -= 2;
    }

    bitmap *blocks_bitmap = new bitmap(BLOCKS_PER_GROUP);
    fd.seekg(cursor(group_no, start_blocks_count, remaining_blocks));
    fd.write(reinterpret_cast<char *>(blocks_bitmap->get_bitmap()),
             blocks_bitmap->get_size());
    remaining_blocks -= 1;

    bitmap *inodes_bitmap = new bitmap(inodes_per_group);
    fd.seekg(cursor(group_no, start_blocks_count, remaining_blocks));
    fd.write(reinterpret_cast<char *>(inodes_bitmap->get_bitmap()),
             inodes_bitmap->get_size());
    remaining_blocks -= 1;

    inode *inode_table = (inode *)malloc(inodes_per_group);
    fd.seekg(cursor(group_no, start_blocks_count, remaining_blocks));
    fd.write(reinterpret_cast<char *>(&inode_table), inodes_per_group);
    remaining_blocks -= 1;

    char *buffer = (char *)calloc(remaining_blocks, block_size);
    fd.seekg(cursor(group_no, start_blocks_count, remaining_blocks));
    fd.write(buffer, remaining_blocks * block_size);
  }

  //
  // fd.seekg(block_size);
  // bitmap *inodes_bitmap = new bitmap(inodes_count);
  // bitmap *blocks_bitmap = new bitmap(blocks_count);
  // std::cout << "blocks_size: " << sizeof(u8) * blocks_bitmap->get_size();
  //
  // fd.write(reinterpret_cast<char *>(blocks_bitmap->get_bitmap()),
  //          blocks_bitmap->get_size());

  fd.close();
}
