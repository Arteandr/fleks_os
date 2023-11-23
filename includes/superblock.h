#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

#include "shared.h"
#include <iostream>
#define SUPER_OFFSET 1024 // Смещение суперблока
#define SUPER_MAGIC 0xEF53

typedef struct super_block {
  u32 s_inodes_count; // Общее количество inode
  u32 s_blocks_count; // Общее количество блоков в ФС
  u32 s_free_indes_count; // Количество свободных inode
  u32 s_block_size;       // Размер блока
  u32 s_blocks_per_group; // Количество блоков в группе
  u32 s_first_data_block; // Первый блок данных
  u32 s_inodes_per_group; // Количество inode в группе
  u16 s_magic;            // Идентификатор ФС
} super_block;
#endif // !SUPERBLOCK_H
