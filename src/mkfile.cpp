#include "../includes/mkfile.h"
#include "../includes/os_status.h"

int MakeFileCommand::execute(std::vector<std::string> args) {
  this->fs->make_file("test.txt");
  return OS_SUCCESS;
};
