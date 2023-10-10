#include "includes/console.h"
#include "includes/executor.h"
#include "includes/os_status.h"
#include <bits/types/time_t.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#define OS_NAME "fleksOS"
using namespace std;

void clear() { system("clear"); }

std::string current_time() {
  std::ostringstream oss;

  time_t now = time(0);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  int h = timeinfo.tm_hour;
  int m = timeinfo.tm_min;
  int s = timeinfo.tm_sec;

  oss << "[" << std::setfill('0') << std::setw(2) << h << ":"
      << std::setfill('0') << std::setw(2) << m << ":" << std::setfill('0')
      << std::setw(2) << s << "]";

  return oss.str();
}

void start() {
  clear();
  Executor *executor = new Executor();

  bool mainLoop = true;
  while (mainLoop) {
    std::string curr_time = current_time();
    cout << "\x1B[31m" << curr_time << " " << OS_NAME << "@"
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

void logo() {
  clear();
  const char *logo = "  ,-.       _,---._ __  / \\\n"
                     " /  )    .-'       `./ /   \\\n"
                     "(  (   ,'            `/    /|\n"
                     " \\  `-'             \\'\\   / |\n"
                     "  `.              ,  \\ \\ /  |\n"
                     "   /`.          ,'-`----Y   |\n"
                     "  (            ;        |   '\n"
                     "  |  ,-.    ,-'         |  /\n"
                     "  |  | (   |    fleksOS | /\n"
                     "  )  |  \\  `.___________|/\n"
                     "  `--'   `--'\n";

  cout << logo << std::endl
       << std::endl
       << "Для продолжения нажмите любую кнопку...";
  system("stty raw");
  getchar();
  system("stty cooked");
}

int main() {
  logo();
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
