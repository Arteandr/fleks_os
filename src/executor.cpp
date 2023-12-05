#include "../includes/executor.h"
#include "../includes/bm.h"
#include "../includes/cat.h"
#include "../includes/chmod.h"
#include "../includes/clear.h"
#include "../includes/cp.h"
#include "../includes/exit.h"
#include "../includes/ls.h"
#include "../includes/mkfile.h"
#include "../includes/os_status.h"
#include "../includes/rename.h"
#include "../includes/rm.h"
#include "../includes/useradd.h"
#include "../includes/users.h"
#include "../includes/write.h"
#include "info.h"
#include <string>
#include <utility>
#include <vector>

Executor::Executor(FS &fs) {
  struct {
    std::string alias;
    Command *cmd;
  } all_commands[] = {
      {"clear", new ClearCommand(fs)},     {"exit", new ExitCommand(fs)},
      {"ls", new ListCommand(fs)},         {"file", new MakeFileCommand(fs)},
      {"rename", new RenameCommand(fs)},   {"bm", new BmCommand(fs)},
      {"rm", new RemoveCommand(fs)},       {"cp", new CopyCommand(fs)},
      {"vedit", new WriteCommand(fs)},     {"cat", new CatCommand(fs)},
      {"usr", new UsersCommand(fs)},       {"chmod", new ChmodCommand(fs)},
      {"useradd", new UserAddCommand(fs)}, {"info", new InfoCommand(fs)}};
  int commands_count = sizeof(all_commands) / sizeof(all_commands[0]);
  for (int i = 0; i < commands_count; i++) {
    std::pair<std::string, Command *> p =
        std::make_pair(all_commands[i].alias, all_commands[i].cmd);
    this->cmds.insert(p);
  }
};
// TODO: почистить
Executor::~Executor() {}

int Executor::execute(std::string cmd, std::vector<std::string> args) {
  if (this->cmds.count(cmd)) {
    int status = this->cmds.at(cmd)->execute(args);
    return status;
  } else
    return OS_ERROR;
};
