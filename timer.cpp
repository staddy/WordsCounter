#include "timer.hpp"

#include <iostream>

Timer::Timer(std::string &&name) : m_name(std::move(name)) {
  m_begin = std::chrono::steady_clock::now();
  std::cout << m_name << ": timer is started\n";
}

long long Timer::elapsedMilliseconds() const {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now() - m_begin)
      .count();
}

Timer::~Timer() {
  std::cout << m_name << ": " << elapsedMilliseconds() << " milliseconds\n";
}
