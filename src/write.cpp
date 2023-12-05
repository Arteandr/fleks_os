#include "../includes/write.h"
#include "../includes/os_status.h"
#include "../includes/vedit.hpp"
#include "fs.h"
#include <cstring>

int WriteCommand::execute(std::vector<std::string> args) {
  if (args.size() != 1)
    return OS_SUCCESS;
  void *buffer;
  size_t read_size = this->fs.read_file(args[0].c_str(), buffer);
  if (read_size == EOF) {
    fs.log("У вас нет прав на выполнение этой команды", LogLevel::error);
    return OS_SUCCESS;
  }

  auto veditor = new VisualEditor(args[0]);
  veditor->open(read_size > 0 ? (char *)buffer : " ");
  const bool save = veditor->run();
  if (save) {
    char *buffer = veditor->get_lines();
    this->fs.write_file(args[0].c_str(), (void *)buffer, std::strlen(buffer));
  }

  delete veditor;

  return OS_SUCCESS;
};
