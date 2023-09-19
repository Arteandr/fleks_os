#include "../includes/exit.h"
#include "../includes/os_status.h"

int ExitCommand::execute(std::vector<std::string> args) { return OS_EXIT; };
