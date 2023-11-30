#ifndef USERS_CMD_H
#define USERS_CMD_H

#include "command.h"
class UsersCommand : public Command {
  virtual int execute(std::vector<std::string>);

public:
  UsersCommand(FS *fs) : Command(fs){};
};
#endif // !DEBUG
