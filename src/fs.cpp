#include "../includes/fs.h"
#include "../includes/superblock.h"
#include "utils.hpp"
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/mman.h>
#include <sys/types.h>

FS::FS(std::string filename) {
  this->fd.open(filename.c_str(),
                std::ios::in | std::ios::out | std::ios::binary);
  if (!this->fd.is_open()) {
    std::string error = "Ошибка при открытии файла ФС - \"" + filename + "\"";
    throw std::runtime_error(error);
  }

  // Читаем суперблок
  super_block sb;
  if (!this->fd.read(reinterpret_cast<char *>(&sb), sizeof(sb))) {
    FS::log("Ошибка чтения суперблока", LogLevel::error);
    return;
  }
  if (sb.s_magic != SUPER_MAGIC) {
    FS::log("Магическое число задано неверно", LogLevel::error);
    return;
  }
  this->superblock = &sb;
  const u8 groups_count =
      this->superblock->s_blocks_count / this->superblock->s_blocks_per_group;

  // Читаем GDT
  group_desc *gdt = new group_desc[groups_count];
  fd.seekg(this->superblock->s_block_size * 1);
  for (size_t i = 0; i < groups_count; ++i) {
    group_desc gd;
    fd.read(reinterpret_cast<char *>(&gd), sizeof(gd));
    gdt[i] = gd;
  }

  this->info();
}

// fs_size - Размер ФС в байтах
// block_size - Размер блока в байтах
void FS::format(size_t fs_size, size_t block_size) {
  FS::log("Запуск форматирования файловой системы");
  FS::log("Размер ФС (байт): " + std::to_string(fs_size));
  FS::log("Размер блока (байт): " + std::to_string(block_size));
  // Высчитываем необходимое количество блоков
  u32 blocks_count = fs_size / block_size;
  FS::log("Количество блоков: " + std::to_string(blocks_count));
  // Количество inode в группе
  u32 inodes_per_group = block_size * 8;
  FS::log("Количество inode в каждой группе: " +
          std::to_string(inodes_per_group));
  // Количество групп блоков
  u32 groups_count = ceil((double)blocks_count / BLOCKS_PER_GROUP);
  FS::log("Количество групп блоков: " + std::to_string(groups_count));
  // Общее количество inode
  u32 inodes_count = inodes_per_group * groups_count;
  FS::log("Общее количестов inode: " + std::to_string(inodes_count));

  FS::log("Инициализирование суперблока");
  super_block sb;
  sb.s_inodes_count = inodes_count;
  sb.s_blocks_count = blocks_count;
  sb.s_free_indes_count = inodes_count;
  sb.s_block_size = block_size;
  sb.s_blocks_per_group = BLOCKS_PER_GROUP;
  sb.s_inodes_per_group = inodes_per_group;
  sb.s_magic = SUPER_MAGIC;

  FS::log("Инициализирование таблицы описателей групп");
  group_desc *group_desc_table =
      (group_desc *)calloc(groups_count, sizeof(group_desc));

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

  FS::log("Начало форматирования групп блоков");
  fd.seekg(0, std::ios::beg);
  for (size_t group_no = 1; group_no <= groups_count; group_no++) {
    size_t remaining_blocks = BLOCKS_PER_GROUP;
    // Если последняя группа не вмещается полностью получаем остаток блоков
    // для последней группы
    if (blocks_count % BLOCKS_PER_GROUP != 0 && group_no == groups_count)
      remaining_blocks = blocks_count % BLOCKS_PER_GROUP;
    FS::log(group_no,
            "Количество блоков в группе - " + std::to_string(remaining_blocks));
    const size_t start_blocks_count = remaining_blocks;

    if (group_no == 1) {
      FS::log(cursor(group_no, start_blocks_count, remaining_blocks) /
                  block_size,
              group_no, "Запись суперблока");
      if (!fd.write(reinterpret_cast<char *>(&sb), sizeof(super_block))) {
        FS::log(group_no, "Ошибка записи суперблока", LogLevel::error);
        return;
      }
      remaining_blocks -= 2;
    }

    bitmap *blocks_bitmap = new bitmap(BLOCKS_PER_GROUP);
    fd.seekg(cursor(group_no, start_blocks_count, remaining_blocks),
             std::ios::beg);
    FS::log(cursor(group_no, start_blocks_count, remaining_blocks) / block_size,
            group_no, "Запись битовой карты блоков");
    if (!fd.write(reinterpret_cast<char *>(blocks_bitmap->get_bitmap()),
                  blocks_bitmap->get_size())) {
      FS::log(group_no, "Ошибка записи битовой карты блоков", LogLevel::error);
      return;
    }
    group_desc_table[group_no - 1].bg_block_bitmap =
        cursor(group_no, start_blocks_count, remaining_blocks) / block_size;
    FS::log(group_no, "Запись bg_block_bitmap: " +
                          std::to_string(cursor(group_no, start_blocks_count,
                                                remaining_blocks) /
                                         block_size));
    remaining_blocks -= 1;
    FS::log(group_no, "Запись блока block_bitmap: " +
                          std::to_string(cursor(group_no, start_blocks_count,
                                                remaining_blocks) /
                                         block_size));

    bitmap *inodes_bitmap = new bitmap(inodes_per_group);
    fd.seekg(cursor(group_no, start_blocks_count, remaining_blocks),
             std::ios::beg);
    if (!fd.write(reinterpret_cast<char *>(inodes_bitmap->get_bitmap()),
                  inodes_bitmap->get_size())) {
      FS::log(group_no, "Ошибка записи битовой карты inode", LogLevel::error);
      return;
    }

    FS::log(cursor(group_no, start_blocks_count, remaining_blocks) / block_size,
            group_no, "] Запись битовой карты inode");

    group_desc_table[group_no - 1].bg_inode_bitmap =
        cursor(group_no, start_blocks_count, remaining_blocks) / block_size;
    FS::log(group_no, "Запись bg_inode_bitmap: " +
                          std::to_string(cursor(group_no, start_blocks_count,
                                                remaining_blocks) /
                                         block_size));
    remaining_blocks -= 1;

    inode *inode_table = (inode *)calloc(inodes_per_group, sizeof(inode));
    fd.seekg(cursor(group_no, start_blocks_count, remaining_blocks),
             std::ios::beg);
    FS::log(cursor(group_no, start_blocks_count, remaining_blocks) / block_size,
            group_no, "Запись таблицы inode");
    for (int i = 0; i < inodes_per_group; ++i) {
      if (!fd.write(reinterpret_cast<char *>(&inode_table[i]), sizeof(inode))) {
        FS::log(group_no, "Ошибка записи таблицы inode", LogLevel::error);
        FS::log(strerror(errno), LogLevel::error);
        return;
      }
    }

    group_desc_table[group_no - 1].bg_inode_table =
        cursor(group_no, start_blocks_count, remaining_blocks) / block_size;
    FS::log(group_no, "Запись bg_inode_table: " +
                          std::to_string(cursor(group_no, start_blocks_count,
                                                remaining_blocks) /
                                         block_size));
    remaining_blocks -= 1;

    FS::log(cursor(group_no, start_blocks_count, remaining_blocks) / block_size,
            group_no, "Начало записи блоков данных");
    char *buffer = (char *)calloc(remaining_blocks, block_size);
    fd.seekg(cursor(group_no, start_blocks_count, remaining_blocks),
             std::ios::beg);
    fd.write(buffer, remaining_blocks * block_size);
    FS::log(cursor(group_no, start_blocks_count, remaining_blocks) /
                    block_size +
                remaining_blocks,
            group_no, "Конец записи блоков данных");
    FS::log(group_no,
            "Размер файла ФС - " + std::to_string(fd.tellg() / 1024) +
                " (кб) " + std::to_string(fd.tellg() / 1024 / 1024) + " (мб)",
            LogLevel::warning);
    delete blocks_bitmap;
    delete inodes_bitmap;
    delete inode_table;
    delete buffer;
  }

  fd.seekg(block_size * 1, std::ios::beg);
  FS::log(1, 1, "Запись таблицы дескрипторов групп");
  for (size_t i = 0; i < groups_count; ++i) {
    FS::log(1, 1, "Запись дескриптора для группы №" + std::to_string(i));
    FS::debug("BG_INODE_TABLE: " +
              std::to_string(group_desc_table[i].bg_inode_table));
    if (!fd.write(reinterpret_cast<char *>(&group_desc_table[i]),
                  sizeof(group_desc))) {
      FS::log(0, "Ошибка записи таблицы дескрипторов групп", LogLevel::error);
      FS::log(strerror(errno), LogLevel::error);
      return;
    }
  }

  FS::log("Файловая система успешно установлена");
  FS::debug("Для продолжения нажмите любую кнопку...");

  fd.close();
}

void FS::debug(std::string message) { FS::log(message, LogLevel::warning); }

void FS::log(u32 block_no, u32 group_no, std::string message,
             LogLevel log_level) {
  std::string group_prefix = "[group: " + std::to_string(group_no) + "] ";
  std::string block_prefix = "[block: " + std::to_string(block_no) + "] ";
  FS::log(group_prefix + block_prefix + message, log_level);
}

void FS::log(u32 group_no, std::string message, LogLevel log_level) {
  std::string group_prefix = "[group: " + std::to_string(group_no) + "] ";
  FS::log(group_prefix + message, log_level);
}

void FS::log(std::string message, LogLevel log_level, bool new_line) {
  std::string current_time = utils::current_time();
  std::string prefix = "", color = "";

  switch (log_level) {
  case LogLevel::info:
    prefix = "[info]";
    color = "\x1B[32m";
    break;
  case LogLevel::warning:
    prefix = "[warning]";
    color = "\x1B[33m";
    break;
  case LogLevel::error:
    prefix = "[error]";
    color = "\x1B[31m";
    break;
  };

  std::cout << color << current_time << " " << prefix << " " << message
            << "\033[0m";
  if (new_line)
    std::cout << std::endl;
}

void FS::info() {
  if (this->superblock == nullptr)
    return;

  FS::log("Информация о системе");
  FS::log("Общее количество inode - " +
          std::to_string(this->superblock->s_inodes_count));
  FS::log("Общее количество блоков - " +
          std::to_string(this->superblock->s_blocks_count));
  FS::log("Количество свободных inode - " +
          std::to_string(this->superblock->s_free_indes_count));
  FS::log("Размер блока - " + std::to_string(this->superblock->s_block_size));
  FS::log("Количество блоков в группе - " +
          std::to_string(this->superblock->s_blocks_per_group));
  FS::log("Первый блок данных - " +
          std::to_string(this->superblock->s_first_data_block));
  FS::log("Количество inode в группе - " +
          std::to_string(this->superblock->s_inodes_per_group));
  FS::log("Магическое число - " + std::to_string(this->superblock->s_magic));
  std::cout << std::endl;
}
