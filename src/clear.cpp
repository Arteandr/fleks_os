#include "../includes/clear.h"
#include "../includes/os_status.h"
#include <iostream>
#include <ostream>

int ClearCommand::execute(std::vector<std::string> args) {
  system("clear");
  return OS_SUCCESS;
};
