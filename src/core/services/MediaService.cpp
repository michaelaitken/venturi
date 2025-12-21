#include "MediaService.hpp"
#include "../../../app/Logger.hpp"
#include <regex>

namespace venturi::core {

MediaService::MediaService(
    std::shared_ptr<IMediaRepository> repository
) : repository_(std::move(repository))
{}

std::optional<MediaInfo> MediaService::get_media(const std::string& id) const {
  return repository_->find_by_id(id);
}

std::vector<MediaInfo> MediaService::list_all_media() const {
  return repository_->list_all();
}

size_t MediaService::scan_media_directory(
  const std::filesystem::path& path
) {
  std::filesystem::path absolute_path{ std::filesystem::absolute(path) }; 
  LOG_INFO("Scanning media directory: ", absolute_path.string());
  
  size_t count = repository_->scan_directory(absolute_path, 
    [this](const MediaInfo& info) {
      LOG_DEBUG("Found media: ", info.file_path.string());
    }
  );
  
  LOG_INFO("Scan complete. Found ", count, " media files");
  return count;
}

uint64_t MediaService::get_media_size(const std::string& media_id) const {
  auto media = repository_->find_by_id(media_id);
  if (!media) {
    return 0;
  }
  
  return repository_->get_file_size(media->file_path);
}

std::optional<ByteRange> MediaService::parse_range_header(
  const std::string& range_header,
  uint64_t file_size
) const {
  // "bytes=start-end" format
  // "bytes=0-1023", "bytes=1024-", "bytes=-500"
  
  static const std::regex range_regex(
    R"(bytes=(\d*)-(\d*))",
    std::regex::icase
  );
  
  std::smatch matches;
  if (!std::regex_match(range_header, matches, range_regex)) {
    return std::nullopt;
  }
  
  ByteRange range;
  range.total_size = file_size;
  
  const std::string start_str = matches[1].str();
  const std::string end_str = matches[2].str();
  
  if (start_str.empty() && end_str.empty()) {
    return std::nullopt;
  }
  
  if (start_str.empty()) {
    uint64_t suffix_length = std::stoull(end_str);
    range.start = file_size > suffix_length ? 
      file_size - suffix_length : 0;
    range.end = file_size - 1;
  } else {
    range.start = std::stoull(start_str);
    range.end = end_str.empty() ? 
      file_size - 1 : std::stoull(end_str);
  }
  
  if (!range.is_valid()) {
    return std::nullopt;
  }
  
  return range;
}

} // namespace venturi::core