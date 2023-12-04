#include "../includes/chmod.h"
#include "../includes/os_status.h"
#include <string>

int ChmodCommand::execute(std::vector<std::string> args) {
  if (args.size() != 2)
    return OS_SUCCESS;
  u32 access;

  try {
    access = std::stoi(args[1]);
  } catch (std::exception error) {
    return OS_SUCCESS;
  }

  this->fs.chmod(args[0].c_str(), access);

  return OS_SUCCESS;
};
