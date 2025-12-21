#include "Config.hpp"
#include "Logger.hpp"
#include "Application.hpp"
#include <csignal>

#include <iostream>

// Global variables for graceful shutdown
namespace {
  std::sig_atomic_t g_signal{ 0 };

  void signal_handler(int signal) {
    g_signal = signal;
  }
}

int main(int argc, char* argv[]) {
  try {
    // Print banner (Slant)
    std::cout << R"(
 _    __           __             _ 
| |  / /__  ____  / /___  _______(_)
| | / / _ \/ __ \/ __/ / / / ___/ / 
| |/ /  __/ / / / /_/ /_/ / /  / /  
|___/\___/_/ /_/\__/\__,_/_/  /_/   
                                    
High-Performance HTTP Media Server
Version 1.0.0
      )" << std::endl;

    // Load configuration
    venturi::Config config{};


    LOG_INFO("Starting Venturi Media Server");
    LOG_INFO("Configuration:");
    LOG_INFO("  Host: ", config.host);
    LOG_INFO("  Port: ", config.port);
    LOG_INFO("  Media Root: ", config.media_root.string());
    LOG_INFO("  Threads: ", config.thread_count);

    // Register signal handlers
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // Create application and start services
    venturi::Application app{ config };
    app.start_services();

    LOG_INFO("Server is running. Press Ctrl+C to stop.");

    while (g_signal == 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    app.stop_services();
  }
  catch (const std::exception& ex) {
    LOG_ERROR("Fatal error: ", ex.what());
    return 1;
  }

  return 0;
}