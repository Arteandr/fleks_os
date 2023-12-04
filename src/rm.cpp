#include "../includes/rm.h"
#include "../includes/os_status.h"

int RemoveCommand::execute(std::vector<std::string> args) {
  if (args.size() != 1)
    return OS_SUCCESS;

  this->fs.remove(args[0].c_str());

  return OS_SUCCESS;
};
