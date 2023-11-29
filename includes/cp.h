#ifndef COPY_CMD_H
#define COPY_CMD_H

#include "command.h"
class CopyCommand : public Command {
  virtual int execute(std::vector<std::string>);

public:
  CopyCommand(FS *fs) : Command(fs){};
};
#endif // !DEBUG
