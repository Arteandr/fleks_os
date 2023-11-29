#include "../includes/cat.h"
#include "../includes/os_status.h"
#include <iostream>
#include <ostream>

int CatCommand::execute(std::vector<std::string> args) {
  if (args.size() != 1)
    return OS_SUCCESS;

  void *buffer;
  size_t read_size = this->fs->read_file(args[0].c_str(), buffer);

  if (read_size <= 0)
    return OS_SUCCESS;
  std::cout << "total " << read_size << std::endl << std::endl;
  std::cout << static_cast<char *>(buffer) << std::endl;
  return OS_SUCCESS;
};
