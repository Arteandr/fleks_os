#include "../includes/executor.h"
#include "../includes/clear.h"
#include "../includes/exit.h"
#include "../includes/os_status.h"
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

Executor::Executor() {
  struct {
    std::string alias;
    Command *cmd;
  } all_commands[] = {{"clear", new ClearCommand}, {"exit", new ExitCommand}};
  int commands_count = sizeof(all_commands) / sizeof(all_commands[0]);

  for (int i = 0; i < commands_count; i++)
    this->cmds.insert(std::pair<std::string, Command *>(all_commands[i].alias,
                                                        all_commands[i].cmd));
};

Executor::~Executor() {
  // std::map<std::string, Command *>::iterator it = this->cmds.begin(),
  //                                            end = this->cmds.end();

  // for (; it != end; ++it)
  //   delete it->second;
}

int Executor::execute(std::string cmd, std::vector<std::string> args) {
  if (this->cmds.count(cmd))
    return this->cmds[cmd]->execute(args);
  else
    return OS_ERROR;
};
