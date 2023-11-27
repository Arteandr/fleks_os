#ifndef EXIT_CMD_H
#define EXIT_CMD_H

#include "command.h"
class ExitCommand : public Command {
  virtual int execute(std::vector<std::string>);

public:
  ExitCommand(FS *fs) : Command(fs){};
};
#endif // !DEBUG
