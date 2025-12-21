#pragma once
#include <string>
#include <cstdint>
#include <chrono>
#include <filesystem>

namespace venturi::core {

struct MediaInfo {
  std::string id;
  std::filesystem::path file_path;
  std::filesystem::path optimized_path;
  
  std::string mime_type = "video/mp4";
  
  std::chrono::system_clock::time_point created_at;
  std::chrono::system_clock::time_point modified_at;
};

struct ByteRange {
  uint64_t start = 0;
  uint64_t end = 0;
  uint64_t total_size = 0;
  
  bool is_valid() const {
    return start <= end && end < total_size;
  }
  
  uint64_t length() const {
    return end - start + 1;
  }
};

} // namespace venturi::core