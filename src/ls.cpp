#include "../includes/ls.h"
#include "../includes/os_status.h"

int ListCommand::execute(std::vector<std::string> args) {
  this->fs->list();
  return OS_SUCCESS;
}
