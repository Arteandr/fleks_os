#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
#include <vector>
class Console {
public:
  static std::string password();
  static std::vector<std::string> prompt();
};

#endif
