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
}

// fs_size - Размер ФС в байтах
// block_size - Размер блока в байтах
void FS::format(size_t fs_size, size_t block_size) {
  std::cout << "Размер ФС (байт): " << fs_size
            << " (мбайт): " << fs_size / 1024 / 1024 << std::endl;
  std::cout << "Размер блока (байт): " << block_size
            << " (кбайт): " << block_size / 1024 << std::endl;
  // Высчитываем необходимое количество блоков
  u32 blocks_count = fs_size / block_size;
  std::cout << "Количество блоков: " << blocks_count << std::endl;
  // Количество inode в группе
  u32 inodes_per_group = block_size * 8;
  std::cout << "Количество inode в группе: " << inodes_per_group << std::endl;
  // Количество групп блоков
  u32 groups_count = ceil((double)blocks_count / BLOCKS_PER_GROUP);
  std::cout << "Количество групп блоков: " << groups_count << std::endl;
  // Общее количество inode
  u32 inodes_count = inodes_per_group * groups_count;
  std::cout << "Общее количество inode: " << inodes_count << std::endl;

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

  std::fstream fd = std::fstream(FS_FILENAME, std::ios::out | std::ios::binary);
  if (!fd.is_open()) {
    std::string error =
        "Ошибка при открытии файла ФС - \"" + std::string(FS_FILENAME) + "\"";
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
    std::cout << "[" << group_no << "] Количество блоков - " << remaining_blocks
              << std::endl;
    const size_t start_blocks_count = remaining_blocks;

    if (group_no == 1) {
      fd.write(reinterpret_cast<char *>(&sb), sizeof(*sb));
      std::cout << "[" << group_no << "] Запись суперблока" << std::endl;
      fd.seekg(block_size, std::ios::beg);
      fd.write(reinterpret_cast<char *>(&group_desc_table),
               sizeof(*group_desc_table));
      std::cout << "[" << group_no << "] Запись bgt" << std::endl;
      fd.seekg(block_size * 2, std::ios::beg);
      remaining_blocks -= 2;
    }

    bitmap *blocks_bitmap = new bitmap(BLOCKS_PER_GROUP);
    fd.seekg(cursor(group_no, start_blocks_count, remaining_blocks),
             std::ios::beg);
    fd.write(reinterpret_cast<char *>(blocks_bitmap->get_bitmap()),
             blocks_bitmap->get_size());
    std::cout << "[" << group_no << "] Запись blocks_bitmap" << std::endl;
    remaining_blocks -= 1;

    bitmap *inodes_bitmap = new bitmap(inodes_per_group);
    fd.seekg(cursor(group_no, start_blocks_count, remaining_blocks),
             std::ios::beg);
    fd.write(reinterpret_cast<char *>(inodes_bitmap->get_bitmap()),
             inodes_bitmap->get_size());
    std::cout << "[" << group_no << "] Запись inodes_bitmap" << std::endl;
    remaining_blocks -= 1;

    inode *inode_table = (inode *)malloc(inodes_per_group);
    fd.seekg(cursor(group_no, start_blocks_count, remaining_blocks),
             std::ios::beg);
    fd.write(reinterpret_cast<char *>(&inode_table), inodes_per_group);
    std::cout << "[" << group_no << "] Запись inodes_table" << std::endl;
    remaining_blocks -= 1;

    std::cout << "[" << group_no
              << "] Запись buffer. Осталось блоков: " << remaining_blocks
              << std::endl;
    char *buffer = (char *)calloc(remaining_blocks, block_size);
    fd.seekg(cursor(group_no, start_blocks_count, remaining_blocks),
             std::ios::beg);
    fd.write(buffer, remaining_blocks * block_size);
    std::cout << "[" << group_no << "] Запись buffer окончена" << std::endl;
    std::cout << std::endl;

    std::cout << "File size: " << fd.tellg() / 1024 / 1024 << std::endl;

    delete blocks_bitmap;
    delete inodes_bitmap;
    delete inode_table;
    delete buffer;
  }

  fd.close();
}
