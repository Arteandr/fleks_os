#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
#include <vector>

struct ConsoleInput {
  std::string cmd;
  std::vector<std::string> args;
};

class Console {
public:
  static std::string password();
  static ConsoleInput *prompt();
};

#endif
