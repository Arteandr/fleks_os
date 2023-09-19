#ifndef CLEAR_CMD_H
#define CLEAR_CMD_H

#include "command.h"
class ClearCommand : public Command {
  virtual int execute(std::vector<std::string>);
};
#endif // !DEBUG
