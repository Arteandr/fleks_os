#include "includes/console.h"
#include "includes/executor.h"
#include "includes/os_status.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#define OS_NAME "fleksOS"
using namespace std;

void clear() { system("clear"); }

void start() {
  clear();
  Executor *executor = new Executor();

  bool mainLoop = true;
  while (mainLoop) {
    cout << "\x1B[31m" << OS_NAME << "@"
         << "hwndrer"
         << ":~$ \033[0m";
    ConsoleInput *input = Console::prompt();
    if (input->cmd.length() > 0)
      switch (executor->execute(input->cmd, input->args)) {
      case OS_ERROR:
        printf("Команда '%s' не найдена\n", input->cmd.c_str());
        break;
      case OS_EXIT:
        mainLoop = false;
        clear();
        break;
      }
  }
}

int main() {
  bool loop = true;

  while (loop) {
    clear();
    cout << "\x1B[31m"
         << "Доступные действия: "
         << "\033[0m" << endl;
    cout << "1 - Запустить систему" << endl;
    cout << "2 - Выполнить установку системы" << endl;
    cout << "3 - Выход" << endl << endl;
    cout << "\x1B[31m"
         << "Ваш выбор: "
         << "\033[0m";

    ConsoleInput *input = Console::prompt();
    if (input->cmd.length() > 0) {
      switch (atoi(input->cmd.c_str())) {
      case 1:
        loop = false;
        start();
        break;
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
