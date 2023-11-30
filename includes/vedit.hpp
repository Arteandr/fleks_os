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

  bool need_save = false;

  void append(std::string &line);
  void remove(int);
  void insert(std::string, int);
  void update();
  void status_line();
  void draw();
  size_t lines_size();
  std::string tabs(std::string &line);
  void input(int c);
  void save();

  void up();
  void left();
  void right();
  void down();

public:
  VisualEditor(const std::string &filename);
  void open(const char *buffer);
  bool run();
  char *get_lines();
};
#endif // !VEDIT_H
