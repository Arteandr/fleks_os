#include "../includes/console.h"
#include <cstdio>
#include <cstring>
#include <ios>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
using namespace std;

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
  vector<string> cmd;
  string tmp;

  cmd.clear();
  char i[256];
  fgets(i, sizeof(i), stdin);
  char *token = strtok(i, " ");
  while (token != nullptr) {
    trim(token);
    cmd.push_back(token);
    token = strtok(nullptr, " ");
  }

  cin.sync();
  cin.clear();
  ConsoleInput *input = new ConsoleInput();
  input->cmd = cmd[0];
  cmd.erase(cmd.begin());
  input->args = cmd;

  return input;
}
