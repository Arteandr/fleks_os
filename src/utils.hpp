#ifndef UTILS_H
#define UTILS_H

#include <iomanip>
#include <string>
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
} // namespace utils

#endif // !UTILS_H
