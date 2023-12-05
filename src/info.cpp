#include "../includes/info.h"
#include "../includes/os_status.h"

int InfoCommand::execute(std::vector<std::string> args) {
  this->fs.info();
  return OS_SUCCESS;
};
