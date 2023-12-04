#include "../includes/useradd.h"
#include "../includes/console.h"
#include "../includes/os_status.h"
#include "utils.hpp"

int UserAddCommand::execute(std::vector<std::string> args) {
  std::string curr_time;
  std::string login, password;
  ConsoleInput *input = nullptr;
  std::string error = "";

  bool main_loop = true;
  while (main_loop) {
    system("clear");
    curr_time = utils::current_time();
    if (error.length() > 0)
      fs.log(error, LogLevel::error);

    fs.log("Введите имя пользователя: ", LogLevel::info, false);
    input = Console::prompt();
    if (input->cmd.length() < 1 || input->cmd.length() > USER_LOGIN_MAX)
      continue;
    else
      login = input->cmd;

    error.clear();
    break;
  }

  while (main_loop) {
    system("clear");
    curr_time = utils::current_time();
    FS::log("Имя пользователя: \x1B[31m" + login + "\033[0m\n");
    if (error.length() > 0)
      fs.log(error, LogLevel::error);

    fs.log("Введите пароль пользователя: ", LogLevel::info, false);
    input = Console::prompt();
    if (input->cmd.length() < 1 || input->cmd.length() > 15)
      continue;
    else
      password = input->cmd;

    error.clear();
    break;
  }

  this->fs.add_user(login.c_str(), password.c_str());
  return OS_SUCCESS;
};
