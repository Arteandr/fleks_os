#ifndef INFO_CMD_H
#define INFO_CMD_H

#include "command.h"
class InfoCommand : public Command {
  virtual int execute(std::vector<std::string>);

public:
  InfoCommand(FS &fs) : Command(fs){};
};
#endif // !DEBUG
