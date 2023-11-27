#ifndef MKFILE_CMD_H
#define MKFILE_CMD_H

#include "command.h"
class MakeFileCommand : public Command {
  virtual int execute(std::vector<std::string>);

public:
  MakeFileCommand(FS *fs) : Command(fs){};
};
#endif // !MKFILE_CMD_H
