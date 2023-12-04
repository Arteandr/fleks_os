#ifndef WRITE_CMD_H
#define WRITE_CMD_H

#include "command.h"
class WriteCommand : public Command {
  virtual int execute(std::vector<std::string>);

public:
  WriteCommand(FS &fs) : Command(fs){};
};
#endif // !DEBUG
