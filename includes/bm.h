
#ifndef BM_CMD_H
#define BM_CMD_H

#include "command.h"
class BmCommand : public Command {
  virtual int execute(std::vector<std::string>);

public:
  BmCommand(FS *fs) : Command(fs){};
};
#endif // !DEBUG
