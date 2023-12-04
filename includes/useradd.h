#ifndef USERADD_CMD_H
#define USERADD_CMD_H

#include "command.h"
class UserAddCommand : public Command {
  virtual int execute(std::vector<std::string>);

public:
  UserAddCommand(FS &fs) : Command(fs){};
};
#endif // !DEBUG
