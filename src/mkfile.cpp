#include "../includes/mkfile.h"
#include "../includes/os_status.h"

int MakeFileCommand::execute(std::vector<std::string> args) {
  if (args.size() < 1)
    return OS_SUCCESS;

  this->fs.make_file(args[0].c_str(), args[0].length());
  return OS_SUCCESS;
};
