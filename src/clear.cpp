#include "../includes/clear.h"
#include "../includes/os_status.h"

int ClearCommand::execute(std::vector<std::string> args) {
  system("clear");
  return OS_SUCCESS;
};
