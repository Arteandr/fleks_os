#include "../includes/vedit.hpp"
#include <cstring>
#include <curses.h>
#include <ncurses.h>
#include <sstream>
#include <string>

VisualEditor::VisualEditor(const std::string &filename) {
  x = y = 0;
  mode = 'n';
  status = " NORMAL";
  this->filename = filename;

  initscr();
  noecho();
  cbreak();
  keypad(stdscr, true);
  use_default_colors();
  start_color();
}

void VisualEditor::remove(int number) { lines.erase(lines.begin() + number); }

void VisualEditor::insert(std::string line, int number) {
  line = tabs(line);
  lines.insert(lines.begin() + number, line);
}

void VisualEditor::append(std::string &line) {
  line = this->tabs(line);
  this->lines.push_back(line);
}

std::string VisualEditor::tabs(std::string &line) {
  std::size_t tab = line.find('\t');
  return tab == line.npos ? line : this->tabs(line.replace(tab, 1, " "));
}

void VisualEditor::open(const char *buffer) {
  auto splited = split_by_newline(buffer);
  for (auto line : splited)
    this->append(line);
}

bool VisualEditor::run() {
  while (mode != 'q') {
    this->update();
    this->status_line();
    this->draw();
    int c = getch();
    this->input(c);
  }

  refresh();
  endwin();

  return this->need_save;
}

void VisualEditor::input(int c) {
  if (mode == 'n')
    switch (c) {
    case 107:
      up();
      return;
    case 104:
      left();
      return;
    case 108:
      right();
      return;
    case 106:
      down();
      return;
    }

  switch (mode) {
  case 27:
  case 'n':
    switch (c) {
    case 'q':
      mode = 'q';
      break;
    case 'i':
      mode = 'i';
      break;
    case 'w':
      this->save();
      break;
    }
    break;
  case 'i':
    switch (c) {
    case 27:
      mode = 'n';
      break;
    case 127:
    case KEY_BACKSPACE:
      if (x == 0 && y > 0) {
        x = lines[y - 1].length();
        lines[y - 1] += lines[y];
        remove(y);
        up();
      } else if (x > 0) {
        lines[y].erase(--x, 1);
      }
      break;
    case KEY_DC:
      if (x == lines[y].length() && y != lines.size() - 1) {
        lines[y] += lines[y + 1];
      } else {
        lines[y].erase(x, 1);
      }
      break;
    case KEY_ENTER:
    case 10:
      if (x < lines[y].length()) {
        insert(lines[y].substr(x, lines[y].length() - x), y + 1);
        lines[y].erase(x, lines[y].length() - x);
      } else {
        insert("", y + 1);
      }
      x = 0;
      down();
      break;
    case KEY_BTAB:
    case KEY_CTAB:
    case KEY_STAB:
    case KEY_CATAB:
    case 9:
      lines[y].insert(x, 2, ' ');
      x += 2;
      break;
    default:
      lines[y].insert(x, 1, c);
      ++x;
    }
  }
}

void VisualEditor::save() {
  mode = 'q';
  this->need_save = true;
}

char *VisualEditor::get_lines() {
  std::stringstream ss;
  for (auto line : this->lines)
    ss << line << '\n';
  ss << '\0';
  char *cstr = new char[ss.str().length() + 1];
  std::strcpy(cstr, ss.str().c_str());

  return cstr;
}

void VisualEditor::up() {
  if (y > 0) {
    --y;
  }
  if (x >= lines[y].length()) {
    x = lines[y].length();
  }
  move(y, x);
}

void VisualEditor::left() {
  if (x > 0) {
    --x;
    move(y, x);
  }
}

void VisualEditor::right() {
  if ((int)x <= COLS && x <= lines[y].length() - 1) {
    ++x;
    move(y, x);
  }
}

void VisualEditor::down() {
  if ((int)y < LINES && y < lines.size() - 1) {
    ++y;
  }

  if (x >= lines[y].length()) {
    x = lines[y].length();
  }
  move(y, x);
}

void VisualEditor::status_line() {
  if (mode == 'n')
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
  else
    init_pair(1, COLOR_GREEN, COLOR_BLACK);

  attron(A_REVERSE);
  attron(A_BOLD);
  attron(COLOR_PAIR(1));
  for (size_t i{}; i < COLS; ++i)
    mvprintw(LINES - 1, i, " ");
  mvprintw(LINES - 1, 0, "%s", status.c_str());
  mvprintw(LINES - 1, COLS - section.length(), "%s", section.c_str());
  attroff(COLOR_PAIR(1));
  attroff(A_BOLD);
  attroff(A_REVERSE);
}

void VisualEditor::draw() {
  for (size_t i{}; i < (size_t)LINES - 1; ++i) {
    if (i >= lines.size()) {
      move(i, 0);
      clrtoeol();
    } else {
      mvprintw(i, 0, "%s", lines[i].c_str());
    }
    clrtoeol();
  }
  move(y, x);
}
size_t VisualEditor::lines_size() {
  size_t s = 0;
  for (auto line : this->lines)
    s += line.size();

  return s;
}

void VisualEditor::update() {
  switch (mode) {
  case 'n':
    status = " NORMAL";
    break;
  case 'i':
    status = " INSERT";
    break;
  case 'q':
    break;
  }

  this->section = " SIZE: " + std::to_string(this->lines_size()) +
                  " COLS: " + std::to_string(x) +
                  " | ROWS: " + std::to_string(y) + " | FILENAME: " + filename +
                  " ";
}
std::vector<std::string>
VisualEditor::split_by_newline(const std::string &str) {
  auto result = std::vector<std::string>{};
  auto ss = std::stringstream{str};
  for (std::string line; std::getline(ss, line, '\n');)
    result.push_back(line);

  return result;
}
