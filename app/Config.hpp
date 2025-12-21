#pragma once
#include <string>
#include <cstdint>
#include <filesystem>
#include <thread>

namespace venturi {

struct Config {
  std::string host = "0.0.0.0";
  uint16_t port = 8080;
  uint32_t thread_count = std::thread::hardware_concurrency();

  std::filesystem::path media_root = "media";

  // Temp for when Transcoding jobs are added
  std::filesystem::path transcode_output = "media/optimized"; 
};

} // namespace venturi