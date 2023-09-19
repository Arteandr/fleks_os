#include "includes/console.h"
#include <cstdlib>
#include <iostream>
#include <vector>
using namespace std;

void clear() { system("clear"); }

int main() {
  bool loop = true;

  while (loop) {
    clear();
    cout << "Доступные действия: " << endl;
    cout << "1 - Запустить систему" << endl;
    cout << "2 - Выполнить установку системы" << endl;
    cout << "3 - Выход" << endl << endl;
    cout << "courseworkOS> ";

    vector<string> cmd = Console::prompt();
    if (cmd.size() > 0) {
      switch (atoi(cmd[0].c_str())) {
      default:
        cerr << "Неверный выбор" << endl;
        break;
      }
    }
  }

  return 0;
}
