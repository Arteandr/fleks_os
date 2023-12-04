#ifndef UTILS_H
#define UTILS_H

#include "../includes/shared.h"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
namespace utils {
inline std::string current_time() {
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

inline std::vector<std::string> split_by_newline(const std::string &str) {
  auto result = std::vector<std::string>{};
  auto ss = std::stringstream{str};
  for (std::string line; std::getline(ss, line, '\n');)
    result.push_back(line);

  return result;
}

inline u32 current_time_to_u32() {
  std::time_t current_time = std::time(nullptr);
  u32 current_time_as_u32 = static_cast<u32>(current_time);

  return current_time_as_u32;
}

inline std::string current_time_from_u32(u32 u32_time) {
  std::time_t time = static_cast<std::time_t>(u32_time);
  std::tm *timeInfo = std::localtime(&time);

  std::ostringstream oss;
  oss << std::put_time(timeInfo, "%b %d %H:%M");

  return oss.str();
}

inline std::vector<size_t>
calculate_column_widths(std::vector<std::string> &matrix, size_t rows,
                        size_t cols) {
  std::vector<size_t> max_width(cols, 0);

  for (size_t row = 0; row < rows; ++row) {
    for (size_t col = 0; col < cols; ++col) {
      size_t width = matrix[row * cols + col].length();
      if (width > max_width[col]) {
        max_width[col] = width;
      }
    }
  }

  return max_width;
}

inline void output_aligned_data(std::vector<std::string> &matrix, size_t rows,
                                size_t cols,
                                const std::vector<size_t> &max_width) {
  for (size_t row = 0; row < rows; ++row) {
    for (size_t col = 0; col < cols; ++col) {
      std::cout << std::setw(max_width[col] + 3) << std::left
                << matrix[row * cols + col];
    }
    std::cout << std::endl;
  }
}
} // namespace utils

#endif // !UTILS_H
