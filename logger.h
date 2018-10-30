#pragma once

#include <stdio.h>

enum LoggerLevel {
  DEBUG,
  PROD,
};

struct Logger {
  LoggerLevel level{LoggerLevel::DEBUG};

  template <typename... Args>
  void log(LoggerLevel message_level, const char* fmt, Args... args) {
    if (message_level < level) return;

    std::printf(fmt, args...);
  };

  template <typename... Args>
  void info(const char* fmt, Args... args) {
    log(LoggerLevel::PROD, fmt, args...);
  };

  template <typename... Args>
  void debug(const char* fmt, Args... args) {
    log(LoggerLevel::DEBUG, fmt, args...);
  };
};
