#include "includes/bitmap.h"
#include "includes/console.h"
#include "includes/executor.h"
#include "includes/fs.h"
#include "includes/os_status.h"
#include "includes/superblock.h"
#include "src/utils.hpp"
#include <bits/types/time_t.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#define OS_NAME "fleksOS"
using namespace std;

void clear() { system("clear"); }

void start() {
  clear();
  Executor *executor = new Executor();
  FS *fs = nullptr;

  try {
    fs = new FS(FS_FILENAME);
  } catch (const runtime_error &e) {
    cerr << "[Ошибка] " << e.what() << endl;
    delete fs;
    return;
  }

  bool mainLoop = true;
  while (mainLoop) {
    std::string curr_time = utils::current_time();
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

void install() {
  std::string curr_time;
  u32 fs_size, block_size;
  ConsoleInput *input = nullptr;
  std::string error = "";

  bool mainLoop = true;
  while (mainLoop) {
    clear();
    curr_time = utils::current_time();
    if (error.length() > 0)
      std::cout << curr_time << " [ОШИБКА]: " << error << std::endl
                << std::endl;
    std::cout << curr_time
              << " Введите размер файловой системы в мегабайтах (min:8): ";
    input = Console::prompt();
    if (input->cmd.length() < 1)
      continue;
    else {
      int size = atoi(input->cmd.c_str());
      if (size == 0 || size < 8) {
        error = "Введено неверное число.";
        continue;
      }
      fs_size = atoi(input->cmd.c_str());
    }

    error.clear();
    break;
  }

  while (mainLoop) {
    clear();
    curr_time = utils::current_time();
    std::cout << curr_time << " Размер файловой системы: " << fs_size << " мб"
              << std::endl
              << std::endl;
    if (error.length() > 0)
      std::cout << curr_time << " [ОШИБКА]: " << error << std::endl
                << std::endl;
    std::cout << curr_time << " Введите размер блока в байтах: ";
    input = Console::prompt();
    if (input->cmd.length() < 1)
      continue;
    else {
      int size = atoi(input->cmd.c_str());
      if (size == 0) {
        error = "Введено неверное число.";
        continue;
      }

      switch (size) {
      case 1024:
      case 2048:
      case 4096:
        block_size = size;
        break;
      default:
        error = "Введен неверный размер блока. Допустимые значения: 1024, "
                "2048, 4096";
        continue;
      }
    }

    error.clear();
    break;
  }

  clear();
  FS::format(fs_size * 1024 * 1024, block_size);
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
      case 2:
        loop = false;
        install();
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
