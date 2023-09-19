#include "../includes/console.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

vector<string> Console::prompt() {
  vector<string> cmd;
  string tmp;
  bool end = false;

  cmd.clear();
  do {
    if ((cin >> tmp) && (cin.peek() == '\n') || cin.eof())
      end = true;

    if (!cin.eof())
      cmd.push_back(tmp);
  } while (!end);

  cin.sync();
  cin.clear();

  return cmd;
}
