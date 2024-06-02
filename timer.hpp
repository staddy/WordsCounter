#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <string>

class Timer {
public:
  Timer(std::string &&name = "");
  Timer(const Timer &) = delete;
  Timer(Timer &&) = default;
  Timer &operator=(const Timer &) = delete;
  Timer &operator=(Timer &&) = default;
  ~Timer();
  long long elapsedMilliseconds() const;

private:
  std::string m_name;
  std::chrono::steady_clock::time_point m_begin;
};

#endif // TIMER_H
