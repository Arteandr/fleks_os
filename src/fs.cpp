#include "../includes/fs.h"
#include "../includes/dentry.h"
#include "../includes/superblock.h"
#include "utils.hpp"
#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <iterator>
#include <pstl/glue_execution_defs.h>
#include <stdexcept>
#include <string>
#include <sys/mman.h>
#include <sys/types.h>

FS::FS(std::string filename, bool with_root) {
  this->fd.open(filename.c_str(),
                std::ios::in | std::ios::out | std::ios::binary);
  if (!this->fd.is_open()) {
    std::string error = "Ошибка при открытии файла ФС - \"" + filename + "\"";
    throw std::runtime_error(error);
  }

  // Читаем суперблок
  super_block *sb = new super_block;
  if (!this->fd.read(reinterpret_cast<char *>(sb), sizeof(super_block))) {
    FS::log("Ошибка чтения суперблока", LogLevel::error);
    return;
  }
  if (sb->s_magic != SUPER_MAGIC) {
    FS::log("Магическое число задано неверно", LogLevel::error);
    return;
  }
  this->superblock = sb;
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
  this->gdt = gdt;

  this->log("Файловая система успешно запущена");
  std::cout << std::endl;
  this->info();

  if (with_root) {
    FS::debug("Файловая система загружена с чтением root");
    this->read_inode(1, this->current_directory);
    this->current_directory_i_no = 1;
  }
}

char FS::extend_inode(inode *i_node) {
  auto [group_no, block_no] = this->allocate_block();

  i_node->i_blocks++;
  if (i_node->i_blocks > BLOCKS_COUNT) {
    log("Закончились блоки данных", LogLevel::error);

    i_node->i_blocks--;
    this->free_block(group_no, block_no);
    return 1;
  }

  i_node->i_size += this->superblock->s_block_size;
  i_node->i_block[i_node->i_blocks - 1] = block_no;

  return 0;
}

info_status FS::directory_info(const char *name, u32 inode_no, u16 type) {
  FS::debug("Получение информации о директории");
  info_status stat;

  inode *i;
  this->read_inode(inode_no, i);

  bool exist = false;
  bool found_empty = false;

  const u32 group_no =
      std::ceil(inode_no / this->superblock->s_inodes_per_group);
  u32 block_no = 0;
  char *block = nullptr;
  u32 size = 0;
  dentry *directory = nullptr;
  dentry *view_directories = nullptr;
  u32 view_entries = 0;
  u32 extend_count = 0;

  if ((type & ALL_ENTRIES) == ALL_ENTRIES)
    view_directories = new dentry[50];

  while (block_no < i->i_blocks && !exist && !found_empty) {
    delete[] block;
    this->read_block(group_no, i, block_no, block);
    directory = reinterpret_cast<dentry *>(block);
    size = this->superblock->s_block_size * block_no;
    while (size < i->i_size && !found_empty && !exist &&
           directory->rec_len == sizeof(dentry)) {
      if ((type & SAME_ENTRY) == SAME_ENTRY &&
          strcmp(name, directory->name) == 0 && strlen(directory->name))
        exist = true;
      else if ((type & EMPTY_ENTRY) == EMPTY_ENTRY &&
               directory->file_type == FILE_TYPE_UNKNOWN)
        found_empty = true;
      else {
        if ((type & ALL_ENTRIES) == ALL_ENTRIES && directory->name_len != 0) {
          if (view_entries >= 50 * (extend_count + 1)) {
            dentry *temp = new dentry[view_entries * 2];
            memset(temp, 0, sizeof(dentry) * view_entries * 2);
            memcpy(temp, view_directories, sizeof(dentry) * view_entries);

            delete[] view_directories;
            view_directories = temp;
            extend_count++;
          }

          view_directories[view_entries] = *directory;
          view_entries++;
        }

        directory++;
        size += directory->rec_len;
      }
    }
    block_no++;
  }

  if (exist || found_empty)
    block_no--;

  if ((type & RETURN_BLOCK) == RETURN_BLOCK) {
    stat.block = block;
    stat.directory = directory;
    stat.block_no = block_no;
  } else {
    delete[] block;
    block = nullptr;
    directory = nullptr;
  }

  if ((type & ALL_ENTRIES) == ALL_ENTRIES) {
    stat.directory = view_directories;
    stat.found_count = view_entries;
  }

  stat.empty = found_empty;
  stat.exist = exist;

  return stat;
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
    FS::debug("Размер битовой карты блоков: " +
              std::to_string(blocks_bitmap->get_size()));
    if (!fd.write(reinterpret_cast<char *>((blocks_bitmap->get_bitmap())),
                  sizeof(blocks_bitmap->get_bitmap()))) {
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
    for (size_t i = 0; i < inodes_per_group; ++i) {
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
    remaining_blocks -= (inodes_per_group * sizeof(inode)) / block_size;
    group_desc_table[group_no - 1].bg_first_data_block =
        cursor(group_no, start_blocks_count, remaining_blocks) / block_size;

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

  fd.close();

  FS *fs = new FS(FS_FILENAME);
  auto [group_no, block_no] = fs->allocate_block();
  inode *root = new inode;
  root->i_block[0] = block_no;
  root->i_blocks = 1;
  root->i_mode = S_IROTH | S_IRUSR | S_IWUSR | S_IXUSR;
  root->i_uid = 0;
  root->i_size = fs->superblock->s_block_size;
  fs->make_empty_directory(group_no, 1, 1, root);
  fs->create_inode(1, root);

  fs->read_inode(1, fs->current_directory);
  fs->current_directory_i_no = 1;

  FS::log("Файловая система успешно установлена");
  FS::debug("Для продолжения нажмите любую кнопку...");

  delete fs;
  delete root;
}

void FS::free_inode(u32 inode_no) {
  const u32 group_no =
      std::ceil(inode_no / this->superblock->s_inodes_per_group);

  bitmap *bm = this->get_inode_bitmap(group_no);
  bm->set_bit(inode_no, false);
  this->set_inode_bitmap(group_no, bm);
}

u32 FS::create_inode(inode *i_node) {
  const size_t groups_count =
      this->superblock->s_blocks_count / this->superblock->s_block_size;
  u32 inode_no;
  u32 group_no;
  bitmap *bm;
  for (size_t i = 0; i < groups_count; ++i) {
    bm = this->get_inode_bitmap(i);
    inode_no = bm->search_free();
    if (inode_no > 0) {
      group_no = i;
      break;
    }
  }

  return this->create_inode(inode_no, i_node);
}

u32 FS::create_inode(u32 inode_no, inode *i) {
  const u32 group_no =
      std::ceil(inode_no / this->superblock->s_inodes_per_group);
  FS::debug("Создание иноды в группе: " + std::to_string(group_no));
  bitmap *inode_bitmap = this->get_inode_bitmap(group_no);
  if (inode_bitmap->get_bit(inode_no))
    return 0;

  inode *inode_table = this->get_inode_table(group_no);
  inode_bitmap->set_bit(inode_no, true);
  inode_table[inode_no] = *i;

  this->set_inode_bitmap(group_no, inode_bitmap);
  this->set_inode_table(group_no, inode_table);

  return inode_no;
}

inode *FS::get_inode_table(u32 group_no) {
  const group_desc &group = this->gdt[group_no];
  fd.seekg(group.bg_inode_table * this->superblock->s_block_size,
           std::ios::beg);
  inode *inode_table =
      (inode *)calloc(this->superblock->s_inodes_per_group, sizeof(inode));
  if (!this->fd.read(reinterpret_cast<char *>(inode_table),
                     this->superblock->s_inodes_per_group * sizeof(inode))) {
    log("Ошибка при чтении таблицы inode", LogLevel::error);
    return nullptr;
  }

  return inode_table;
}

bitmap *FS::get_inode_bitmap(size_t group_no) {
  const group_desc &group = this->gdt[group_no];
  fd.seekg(group.bg_inode_bitmap * this->superblock->s_block_size,
           std::ios::beg);
  char *buffer = new char[this->superblock->s_block_size];
  fd.read(buffer, this->superblock->s_block_size);
  bitmap *bm = new bitmap(buffer, this->superblock->s_blocks_per_group);

  return bm;
}

bitmap *FS::get_block_bitmap(size_t block_group_no) {
  const group_desc &group = this->gdt[block_group_no];
  fd.seekg(group.bg_block_bitmap * this->superblock->s_block_size,
           std::ios::beg);
  char *buffer = new char[this->superblock->s_block_size];
  fd.read(buffer, this->superblock->s_block_size);
  bitmap *bm = new bitmap(buffer, this->superblock->s_blocks_per_group);

  return bm;
}

bool FS::set_inode_table(u32 group_no, inode *inode_table) {
  const group_desc &group = this->gdt[group_no];
  fd.seekg(group.bg_inode_table * this->superblock->s_block_size,
           std::ios::beg);
  if (!fd.write(reinterpret_cast<char *>(inode_table),
                this->superblock->s_inodes_per_group * sizeof(inode)))
    return false;

  return true;
}

bool FS::set_inode_bitmap(size_t group_no, bitmap *bm) {
  const group_desc &group = this->gdt[group_no];
  fd.seekg(group.bg_inode_bitmap * this->superblock->s_block_size,
           std::ios::beg);
  if (!fd.write(reinterpret_cast<char *>(bm->get_bitmap()), bm->get_size()))
    return false;

  return true;
}

bool FS::set_block_bitmap(size_t group_no, bitmap *bm) {
  const group_desc &group = this->gdt[group_no];
  fd.seekg(group.bg_block_bitmap * this->superblock->s_block_size,
           std::ios::beg);
  if (!fd.write(reinterpret_cast<char *>(bm->get_bitmap()), bm->get_size()))
    return false;

  return true;
}

void FS::read_inode(u32 inode_no, inode *&i) {
  const u32 group_no =
      std::ceil(inode_no / this->superblock->s_inodes_per_group);
  FS::debug("Чтение inode с номером " + std::to_string(inode_no) +
            " в группе: " + std::to_string(group_no));
  bitmap *bm = this->get_inode_bitmap(group_no);
  if (!bm->get_bit(inode_no)) {
    FS::log("Чтение пустого inode с номером " + std::to_string(inode_no),
            LogLevel::error);
    return;
  }
  // TODO: Добавить проверку на номер инода
  if (inode_no < 1) {
    i = nullptr;
    FS::log("Чтение некорректного inode", LogLevel::error);
    return;
  }

  inode *inode_table = this->get_inode_table(group_no);

  i = &(inode_table[inode_no]);
}

std::pair<u32, u32> FS::allocate_block() {
  const size_t groups_count =
      this->superblock->s_blocks_count / this->superblock->s_block_size;
  size_t block_no;
  size_t group_no;
  bitmap *bm;
  for (size_t i = 0; i < groups_count; ++i) {
    bm = this->get_block_bitmap(i);
    block_no = bm->search_free();
    if (block_no > 0) {
      group_no = i;
      break;
    }
  }
  const group_desc &group = this->gdt[group_no];
  block_no = group.bg_first_data_block + block_no - 1;

  FS::debug("Выделение свободного блока данных");
  FS::debug("Группа: " + std::to_string(group_no));
  FS::debug("Выделенный блок: " + std::to_string(block_no));
  bm->set_bit(block_no, true);
  if (!this->set_block_bitmap(group_no, bm)) {
    FS::log(group_no, "Битовая карта блоков поврежденна", LogLevel::error);
    return {};
  }

  // return {group_no, group.bg_first_data_block + block_no - 1};
  return {group_no, block_no};
}

dentry *FS::make_directory_block() {
  // dentry *directories =
  //     new dentry[this->superblock->s_block_size / sizeof(dentry)];
  dentry *directories = (dentry *)calloc(
      this->superblock->s_block_size / sizeof(dentry), sizeof(dentry));

  dentry *empty_directory = new dentry;
  empty_directory->inode = 0;
  empty_directory->file_type = FILE_TYPE_UNKNOWN;
  empty_directory->name_len = 0;
  empty_directory->rec_len = sizeof(dentry);

  for (size_t i = 0; i < (this->superblock->s_block_size / sizeof(dentry)); ++i)
    directories[i] = *empty_directory;

  delete empty_directory;
  return directories;
}

void FS::make_empty_directory(u32 group_no, u32 inode_no, u32 parent_inode_no,
                              inode *i) {
  dentry *current_directory = new dentry();
  current_directory->inode = inode_no;
  current_directory->file_type = FILE_TYPE_DIR;
  current_directory->name_len = 1;
  current_directory->rec_len = sizeof(dentry);
  memcpy(current_directory->name, ".", 1);

  dentry *parent_directory = new dentry();
  parent_directory->inode = parent_inode_no;
  parent_directory->file_type = FILE_TYPE_DIR;
  parent_directory->name_len = 2;
  parent_directory->rec_len = sizeof(dentry);
  memcpy(parent_directory->name, "..", 2);

  dentry *directories = make_directory_block();

  directories[0] = *current_directory;
  directories[1] = *parent_directory;
  this->write_block(group_no, i, 0, reinterpret_cast<char *>(directories));

  delete[] directories;
  delete current_directory;
  delete parent_directory;
}

void FS::read_block(u32 group_no, inode *i, u32 block_no, char *&buffer) {
  bitmap *bm = this->get_block_bitmap(group_no);
  if (!bm->get_bit(i->i_block[block_no])) {
    log(group_no,
        "Попытка чтения пустого блока с номером: " + std::to_string(block_no),
        LogLevel::error);
    return;
  }

  buffer = new char[this->superblock->s_block_size];
  this->fd.seekg(i->i_block[block_no] * this->superblock->s_block_size,
                 std::ios::beg);
  debug("Текущий курсор при чтении блока номером " +
        std::to_string(i->i_block[block_no]) + " " +
        std::to_string(this->fd.tellg()));
  if (!this->fd.read(buffer, this->superblock->s_block_size)) {
    log("Ошибка при чтении блока: " + std::to_string(block_no),
        LogLevel::error);
    delete[] buffer;
    return;
  }
}

// TODO: fix inode table size
void FS::write_block(u32 group_no, inode *i, u32 block_no, char *buffer) {
  bitmap *bm = this->get_block_bitmap(group_no);
  if (!bm->get_bit(i->i_block[block_no])) {
    log(group_no,
        "Попытка записать в занятый блок с номером " +
            std::to_string(i->i_block[block_no]),
        LogLevel::error);
    return;
  }

  this->fd.seekg(i->i_block[block_no] * this->superblock->s_block_size,
                 std::ios::beg);
  debug("Текущий курсор при записи в блок номер " + std::to_string(block_no) +
        " " + std::to_string(this->fd.tellg()));
  if (!this->fd.write(buffer, this->superblock->s_block_size)) {
    log("Ошибка при записи в блок: " + std::to_string(block_no),
        LogLevel::error);
    return;
  }
}

void FS::free_block(u32 group_no, u32 block_no) {
  bitmap *bm = this->get_block_bitmap(group_no);
  bm->set_bit(block_no, false);
  this->set_block_bitmap(group_no, bm);
}

void FS::debug(std::string message) {
#if DEBUG == 1
  FS::log(message, LogLevel::warning);
#endif
}

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

void FS::make_file(const char *filename) {
  if (std::strlen(filename) > FILE_NAME_LENGTH)
    return;

  info_status stat =
      this->directory_info(filename, this->current_directory_i_no, SAME_ENTRY);
  if (stat.exist)
    return;

  auto [group_no, block_no] = this->allocate_block();
  inode *i_node = new inode;
  i_node->i_blocks = 1;
  i_node->i_block[0] = block_no;
  i_node->i_size = 0;
  i_node->i_uid = this->current_uid;
  u32 inode_no = this->create_inode(i_node);

  info_status directory = this->directory_info(
      nullptr, this->current_directory_i_no, EMPTY_ENTRY | RETURN_BLOCK);

  if (!directory.empty) {
    delete[] directory.block;

    if (this->extend_inode(this->current_directory) == 1) {
      this->free_block(group_no, i_node->i_block[0]);
      this->free_inode(inode_no);
      delete i_node;
      return;
    }

    directory.block = (char *)this->make_directory_block();
    directory.directory = (dentry *)directory.block;
  }

  directory.directory->file_type = FILE_TYPE_FILE;
  directory.directory->inode = inode_no;
  directory.directory->name_len = std::strlen(filename);
  memcpy(directory.directory->name, filename, directory.directory->name_len);
  this->write_block(group_no, this->current_directory, directory.block_no,
                    directory.block);

  delete i_node;
  delete[] directory.block;

  FS::debug("Успешно создан файл");
  FS::debug(filename);
}

std::vector<size_t> calculateColumnWidths(std::vector<std::string> &matrix,
                                          size_t rows, size_t cols) {
  std::vector<size_t> max_width(cols, 0);

  for (size_t row = 0; row < rows; ++row) {
    for (size_t col = 0; col < cols; ++col) {
      size_t width = matrix[row * cols + col].length();
      if (width > max_width[col]) {
        max_width[col] = width;
      }
    }
  }

  return max_width;
}

void outputAlignedData(std::vector<std::string> &matrix, size_t rows,
                       size_t cols, const std::vector<size_t> &max_width) {
  for (size_t row = 0; row < rows; ++row) {
    for (size_t col = 0; col < cols; ++col) {
      std::cout << std::setw(max_width[col] + 3) << std::left
                << matrix[row * cols + col];
    }
    std::cout << std::endl;
  }
}

void FS::list() {
  info_status stat =
      this->directory_info(nullptr, this->current_directory_i_no, ALL_ENTRIES);
  std::cout << "Всего " << stat.found_count << "\n\n";

  if (stat.found_count < 1)
    return;

  std::vector<std::string> data = {"name", "uid", "size"};
  size_t rows = stat.found_count + 1;
  size_t columns = 3;

  for (size_t i = 0; i < stat.found_count; ++i) {
    inode *i_node;
    this->read_inode(stat.directory[i].inode, i_node);
    data.push_back(stat.directory[i].name);
    data.push_back(std::to_string(i_node->i_uid));
    data.push_back(std::to_string(i_node->i_size));
  }

  std::vector<size_t> column_widths =
      calculateColumnWidths(data, rows, columns);

  outputAlignedData(data, rows, columns, column_widths);
}

void FS::info() {
  if (this->superblock == nullptr)
    return;

  FS::log("Информация о системе");
  FS::log("Общий размер ФС: " +
          std::to_string(this->superblock->s_blocks_count *
                         this->superblock->s_block_size) +
          " байт");
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
