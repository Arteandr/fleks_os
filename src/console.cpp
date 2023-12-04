#include "../includes/console.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

void trim(char *str) {
  if (str == nullptr) {
    return;
  }

  char *start = str;
  char *end = str + strlen(str) - 1;

  while (isspace(*start))
    start++;

  if (start > end) {
    str[0] = '\0';
    return;
  }

  while (isspace(*end))
    end--;

  end[1] = '\0';
}

ConsoleInput *Console::prompt() {
  std::vector<std::string> cmd;
  std::string tmp;

  cmd.clear();
  char i[256];
  fgets(i, sizeof(i), stdin);
  char *token = strtok(i, " ");
  while (token != nullptr) {
    trim(token);
    cmd.push_back(token);
    token = strtok(nullptr, " ");
  }

  std::cin.sync();
  std::cin.clear();
  ConsoleInput *input = new ConsoleInput();
  input->cmd = cmd[0];
  cmd.erase(cmd.begin());
  input->args = cmd;

  return input;
}
