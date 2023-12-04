#ifndef CAT_CMD_H
#define CAT_CMD_H

#include "command.h"
class CatCommand : public Command {
  virtual int execute(std::vector<std::string>);

public:
  CatCommand(FS &fs) : Command(fs){};
};
#endif // !DEBUG
