#include "../includes/users.h"
#include "../includes/os_status.h"

int UsersCommand::execute(std::vector<std::string> args) {
  this->fs.users();

  return OS_SUCCESS;
};
