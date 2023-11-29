#include "../includes/write.h"
#include "../includes/os_status.h"
#include "../includes/vedit.hpp"
#include <iostream>
#include <ostream>

int WriteCommand::execute(std::vector<std::string> args) {
  if (args.size() != 2)
    return OS_SUCCESS;
  void *buffer;
  size_t read_size = this->fs->read_file(args[0].c_str(), buffer);
  if (read_size <= 0)
    return OS_SUCCESS;

  // this->fs->write_file(args[0].c_str(), (void *)args[1].c_str(),
  //                      args[1].length());

  auto veditor = new VisualEditor(args[0]);
  veditor->open((char *)buffer);
  veditor->run();

  return OS_SUCCESS;
};
