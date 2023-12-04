#include "../includes/rename.h"
#include "../includes/os_status.h"

int RenameCommand::execute(std::vector<std::string> args) {
  if (args.size() != 2)
    return OS_SUCCESS;

  this->fs.rename(args[0].c_str(), args[1].c_str());
  return OS_SUCCESS;
};
