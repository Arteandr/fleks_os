#ifndef RENAME_CMD_H
#define RENAME_CMD_H

#include "command.h"

class RenameCommand : public Command {
  virtual int execute(std::vector<std::string>);

public:
  RenameCommand(FS &fs) : Command(fs){};
};
#endif
