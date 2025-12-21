#pragma once
#include <iostream>
#include <sstream>
#include <mutex>
#include <chrono>
#include <iomanip>

// (TODO): Replace with 'spdlog'
// Basic Logger taken from another project
class Logger {
public:
  enum class Level { DEBUG, INFO, WARNING, ERROR };

  static Logger& instance() {
    static Logger logger;
    return logger;
  }

  template<typename... Args>
  void log(Level level, Args&&... args) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::cout << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") 
              << "] [" << levelToString(level) << "] ";
    
    (std::cout << ... << args) << std::endl;
  }

private:
  std::mutex mutex_;
  
  const char* levelToString(Level level) {
    switch(level) {
      case Level::DEBUG: return "DEBUG";
      case Level::INFO: return "INFO";
      case Level::WARNING: return "WARN";
      case Level::ERROR: return "ERROR";
      default: return "UNKNOWN";
    }
  }
};

#define LOG_DEBUG(...) Logger::instance().log(Logger::Level::DEBUG, __VA_ARGS__)
#define LOG_INFO(...) Logger::instance().log(Logger::Level::INFO, __VA_ARGS__)
#define LOG_WARN(...) Logger::instance().log(Logger::Level::WARNING, __VA_ARGS__)
#define LOG_ERROR(...) Logger::instance().log(Logger::Level::ERROR, __VA_ARGS__)