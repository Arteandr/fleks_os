#include "includes/console.h"
#include <cstdlib>
#include <iostream>
#include <vector>
using namespace std;

void clear() { system("clear"); }

void start() { clear(); }

int main() {
  bool loop = true;

  while (loop) {
    clear();
    cout << "Доступные действия: " << endl;
    cout << "1 - Запустить систему" << endl;
    cout << "2 - Выполнить установку системы" << endl;
    cout << "3 - Выход" << endl << endl;
    cout << "courseworkOS> ";

    ConsoleInput *input = Console::prompt();
    if (input->cmd.length() > 0) {
      switch (atoi(input->cmd.c_str())) {
      case 3:
        clear();
        loop = false;
        break;
      default:
        cerr << "Неверный выбор" << endl;
        break;
      }
    }
  }

  return 0;
}
