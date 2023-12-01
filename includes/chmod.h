#ifndef CHMOD_CMD_H
#define CHMOD_CMD_H

#include "command.h"
class ChmodCommand : public Command {
  virtual int execute(std::vector<std::string>);

public:
  ChmodCommand(FS *fs) : Command(fs){};
};
#endif // !DEBUG
