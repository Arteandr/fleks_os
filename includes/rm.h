#ifndef REMOVE_CMD_H
#define REMOVE_CMD_H

#include "command.h"
class RemoveCommand : public Command {
  virtual int execute(std::vector<std::string>);

public:
  RemoveCommand(FS *fs) : Command(fs){};
};
#endif // !REMOVE_CMD_H
