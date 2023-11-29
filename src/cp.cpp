#include "../includes/cp.h"
#include "../includes/os_status.h"
#include <iostream>
#include <ostream>

int CopyCommand::execute(std::vector<std::string> args) {
  if (args.size() != 2)
    return OS_SUCCESS;

  this->fs->copy(args[0].c_str(), args[1].c_str(), args[1].length());
  return OS_SUCCESS;
};
