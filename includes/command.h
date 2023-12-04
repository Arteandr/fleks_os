#ifndef COMMAND_H
#define COMMAND_H

#include "fs.h"
#include <string>
#include <vector>
class Command {
public:
  FS &fs;
  Command(FS &_fs) : fs(_fs){};
  virtual int execute(std::vector<std::string>) = 0;
};

#endif
