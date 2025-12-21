#pragma once
#include "../ports/IMediaRepository.hpp"
#include "../entities/MediaInfo.hpp"
#include <memory>
#include <vector>
#include <optional>

namespace venturi::core {

class MediaService {
public:
  MediaService(
    std::shared_ptr<IMediaRepository> repository
  );
  
  std::optional<MediaInfo> get_media(const std::string& id) const;
  
  std::vector<MediaInfo> list_all_media() const;
  
  size_t scan_media_directory(const std::filesystem::path& path);
  
  uint64_t get_media_size(const std::string& media_id) const;

  std::optional<ByteRange> parse_range_header(
    const std::string& range_header,
    uint64_t file_size
  ) const;

private:
  std::shared_ptr<IMediaRepository> repository_;
};

} // namespace venturi::core