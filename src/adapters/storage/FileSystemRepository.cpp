#include "FileSystemRepository.hpp"
#include "../../../app/Logger.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace venturi::adapters {

FileSystemRepository::FileSystemRepository(
  const std::filesystem::path& media_root,
  const std::filesystem::path& optimized_root
) : media_root_(media_root),
  optimized_root_(optimized_root)
{
  // Ensure directories exist
  std::error_code ec;
  std::filesystem::create_directories(media_root_, ec);
  // std::filesystem::create_directories(optimized_root_, ec);
}

std::optional<core::MediaInfo> FileSystemRepository::find_by_id(
  const std::string& id
) const {
  std::shared_lock lock(mutex_);

  auto it = media_map_.find(id);
  if (it == media_map_.end()) {
    return std::nullopt;
  }
  
  return it->second;
}

std::vector<core::MediaInfo> FileSystemRepository::list_all() const {
  std::shared_lock lock(mutex_);
  
  std::vector<core::MediaInfo> result;
  result.reserve(media_map_.size());
  
  for (const auto& [id, info] : media_map_) {
    result.push_back(info);
  }
  
  std::sort(result.begin(), result.end(),
    [](const auto& a, const auto& b) {
      return a.file_path < b.file_path;
    });
  
  return result;
}

size_t FileSystemRepository::scan_directory(
  const std::filesystem::path& path,
  std::function<void(const core::MediaInfo&)> on_found
) {
  size_t count = 0;
  std::error_code ec;
  
  for (const auto& entry : 
      std::filesystem::recursive_directory_iterator(path, ec)) {
    
    if (ec) {
      LOG_WARN("Error scanning directory: ", ec.message());
      ec.clear();
      continue;
    }
    
    if (!entry.is_regular_file(ec)) {
      continue;
    }
    
    if (!is_video_file(entry.path())) {
      continue;
    }
    
    auto info = create_media_info(entry.path());
    
    save(info);
    
    if (on_found) {
      on_found(info);
    }
    
    count++;
  }
  
  return count;
}

void FileSystemRepository::save(const core::MediaInfo& info) {
  std::unique_lock lock(mutex_);
  media_map_[info.id] = info;
}

bool FileSystemRepository::remove(const std::string& id) {
  std::unique_lock lock(mutex_);
  return media_map_.erase(id) > 0;
}

bool FileSystemRepository::exists(const std::string& id) const {
  std::shared_lock lock(mutex_);
  return media_map_.find(id) != media_map_.end();
}

core::MediaInfo FileSystemRepository::create_media_info(
  const std::filesystem::path& file_path
) const {
  core::MediaInfo info;
  
  info.id = generate_media_id(file_path);
  info.file_path = file_path;
  
  std::error_code ec;
  auto ftime = std::filesystem::last_write_time(file_path, ec);
  if (!ec) {
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
      ftime - std::filesystem::file_time_type::clock::now() +
      std::chrono::system_clock::now()
    );
    info.modified_at = sctp;
  }
  
  info.created_at = std::chrono::system_clock::now();
  
  // Set MIME type based on extension
  auto ext = file_path.extension().string();
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
  
  if (ext == ".mp4") {
    info.mime_type = "video/mp4";
  } else if (ext == ".webm") {
    info.mime_type = "video/webm";
  } else if (ext == ".mkv") {
    info.mime_type = "video/x-matroska";
  } else if (ext == ".avi") {
    info.mime_type = "video/x-msvideo";
  } else {
    info.mime_type = "video/mp4"; // Default
  }
  
  return info;
}

std::string FileSystemRepository::generate_media_id(
  const std::filesystem::path& file_path
) const {  
  auto path_str = file_path.string();
  std::hash<std::string> hasher;
  size_t hash = hasher(path_str);
  
  std::ostringstream oss;
  oss << std::hex << std::setfill('0') << std::setw(16) << hash;
  return oss.str();
}

bool FileSystemRepository::is_video_file(
  const std::filesystem::path& file_path
) const {
  static const std::vector<std::string> video_extensions = {
    ".mp4", ".m4v", ".mkv", ".webm", ".avi", ".mov", 
    ".wmv", ".flv", ".mpg", ".mpeg"
  };
  
  auto ext = file_path.extension().string();
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
  
  return std::find(video_extensions.begin(), 
                    video_extensions.end(), 
                    ext) != video_extensions.end();
}

uint64_t FileSystemRepository::get_file_size(const std::filesystem::path& file_path) const {
  std::error_code ec;
  auto size = std::filesystem::file_size(file_path, ec);
  return ec ? 0 : size;
}

} // namespace venturi::adapters