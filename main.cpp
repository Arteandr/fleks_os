#include "includes/console.h"
#include "includes/executor.h"
#include "includes/fs.h"
#include "includes/os_status.h"
#include "src/utils.hpp"
#include <bits/types/time_t.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#define OS_NAME "fleksOS"
using namespace std;

void clear() { system("clear"); }

void start() {
  clear();
  FS *fs = nullptr;

  try {
    fs = new FS(FS_FILENAME, true);
  } catch (const runtime_error &e) {
    FS::log(e.what(), LogLevel::error);
    delete fs;
    return;
  }

  Executor *executor = new Executor(fs);
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
  std::string password;
  ConsoleInput *input = nullptr;
  std::string error = "";

  bool mainLoop = true;
  while (mainLoop) {
    clear();
    curr_time = utils::current_time();
    if (error.length() > 0)
      FS::log(error, LogLevel::error);

    FS::log("Введите размер файловой системы в мегабайтах (min:8): ",
            LogLevel::info, false);
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
    FS::log("Размер файловой системы: \x1B[31m" + std::to_string(fs_size) +
            " Мбайт\033[0m\n");
    if (error.length() > 0)
      FS::log(error, LogLevel::error);

    FS::log("Введите размер блока в байтах: ", LogLevel::info, false);
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
                "2048, 4096.";
        continue;
      }
    }

    error.clear();
    break;
  }

  while (mainLoop) {
    clear();
    curr_time = utils::current_time();
    FS::log("Размер файловой системы: \x1B[31m" + std::to_string(fs_size) +
            " Мбайт\033[0m\n");
    FS::log("Выбранный размер блока: \x1B[31m" + std::to_string(block_size) +
            " КиБ\033[0m\n");
    if (error.length() > 0)
      FS::log(error, LogLevel::error);

    FS::log("Введите пароль суперпользователя (root): ", LogLevel::info, false);
    input = Console::prompt();
    if (input->cmd.length() < 1)
      continue;
    else {
      if (input->cmd.length() > 15) {
        error = "Введено неверное число.";
        continue;
      }
      password = input->cmd;
    }

    error.clear();
    break;
  }

  clear();
  FS::format(fs_size * 1024 * 1024, block_size, password);
  system("stty raw");
  getchar();
  system("stty cooked");

  start();
}

void logo() {
  clear();
  const char *logo = "\x1B[31m  ,-.       _,---._ __  / \\\n"
                     " /  )    .-'       `./ /   \\\n"
                     "(  (   ,'            `/    /|\n"
                     " \\  `-'             \\'\\   / |\n"
                     "  `.              ,  \\ \\ /  |\n"
                     "   /`.          ,'-`----Y   |\n"
                     "  (            ;        |   '\n"
                     "  |  ,-.    ,-'         |  /\n"
                     "  |  | (   |    \x1B[35mfleksOS\x1B[31m | /\n"
                     "  )  |  \\  `.___________|/\n"
                     "  `--'   `--'\n\033[0m";

  cout << logo << std::endl << std::endl;
  cout << "\x1B[35mДля продолжения нажмите любую кнопку...\033[0m";
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
    cout << "\x1B[35m[1] Запустить систему\033[0m" << endl;
    cout << "\x1B[35m[2] Выполнить установку системы\033[0m" << endl;
    cout << "\x1B[35m[3] Выход\033[0m" << endl << endl;
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
