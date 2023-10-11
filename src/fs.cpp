#include "../includes/fs.h"
#include <ios>
#include <stdexcept>

FS::FS(std::string filename) {
  this->fd.open(filename.c_str(), std::ios::out | std::ios::binary);
  if (!this->fd.is_open()) {
    std::string error = "Ошибка при открытии файла ФС - \"" + filename + "\"";
    throw std::runtime_error(error);
  }
}
