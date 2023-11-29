#ifndef VEDIT_H
#define VEDIT_H

#include <cstddef>
#include <string>
#include <vector>

class VisualEditor {
  size_t x, y;
  char mode;
  std::string filename, status, section;
  std::vector<std::string> lines;

  void append(std::string &line);
  void remove(int);
  void insert(std::string, int);
  void update();
  void status_line();
  void draw();
  size_t lines_size();
  std::string tabs(std::string &line);
  std::vector<std::string> split_by_newline(const std::string &str);
  void input(int c);

  void up();
  void left();
  void right();
  void down();

  // void save();

public:
  VisualEditor(const std::string &filename);
  void open(const char *buffer);
  void run();
};
#endif // !VEDIT_H
