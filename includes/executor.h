#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "command.h"
#include <map>
#include <string>
#include <vector>
class Executor {

public:
  std::map<std::string, Command *> cmds;
  Executor(FS &fs);
  ~Executor();
  int execute(std::string cmd, std::vector<std::string> args);
};

#endif
