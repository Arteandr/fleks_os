#ifndef LS_CMD_H
#define LS_CMD_H

#include "command.h"
class ListCommand : public Command {
  virtual int execute(std::vector<std::string>);

public:
  ListCommand(FS *fs) : Command(fs){};
};

#endif // !LS_CMD_H
