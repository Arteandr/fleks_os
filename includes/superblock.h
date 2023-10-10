#include "shared.h"
#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

#define SUPER_OFFSET 1024 // Смещение суперблока
#define SUPER_MAGIC 0xEF53

struct alignas(1024) superblock {
  u32 s_inodes_count; // Количество индексных дескрипторов
  u32 s_blocks_count; // Количество блоков в ФС
  u32 s_block_size;   // Размер блока
  u16 s_magic;        // Идентификатор ФС
};
#endif // !SUPERBLOCK_H