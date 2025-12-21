#pragma once
#include "../../core/ports/IMediaRepository.hpp"
#include <unordered_map>
#include <shared_mutex>
#include <filesystem>

namespace venturi::adapters {

class FileSystemRepository : public core::IMediaRepository {
public:
  explicit FileSystemRepository(
    const std::filesystem::path& media_root,
    const std::filesystem::path& optimized_root
  );
  
  std::optional<core::MediaInfo> find_by_id(
    const std::string& id
  ) const override;
  
  std::vector<core::MediaInfo> list_all() const override;
  
  size_t scan_directory(
    const std::filesystem::path& path,
    std::function<void(const core::MediaInfo&)> on_found
  ) override;
  
  void save(const core::MediaInfo& info) override;
  bool remove(const std::string& id) override;
  bool exists(const std::string& id) const override;
  uint64_t get_file_size(const std::filesystem::path& file_path) const;

private:
  core::MediaInfo create_media_info(
    const std::filesystem::path& file_path
  ) const;
  
  std::string generate_media_id(
    const std::filesystem::path& file_path
  ) const;
  
  bool is_video_file(const std::filesystem::path& file_path) const;
  
  std::filesystem::path media_root_;
  std::filesystem::path optimized_root_;
  
  mutable std::shared_mutex mutex_;
  std::unordered_map<std::string, core::MediaInfo> media_map_;
};

} // namespace venturi::adapters