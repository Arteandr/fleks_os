#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "command.h"
#include <map>
#include <string>
#include <vector>
class Executor {
  std::map<std::string, Command *> cmds;

public:
  Executor();
  ~Executor();
  int execute(std::string cmd, std::vector<std::string> args);
};

#endif
