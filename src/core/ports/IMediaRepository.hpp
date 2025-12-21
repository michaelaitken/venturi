#pragma once
#include "../entities/MediaInfo.hpp"
#include <memory>
#include <vector>
#include <optional>
#include <functional>

namespace venturi::core {

class IMediaRepository {
public:
	virtual ~IMediaRepository() = default;
	
	virtual std::optional<MediaInfo> find_by_id(
		const std::string& id
	) const = 0;
	
	virtual std::vector<MediaInfo> list_all() const = 0;
	
	virtual size_t scan_directory(
		const std::filesystem::path& path,
		std::function<void(const MediaInfo&)> on_found = nullptr
	) = 0;
	
	virtual void save(const MediaInfo& info) = 0;
	
	virtual bool remove(const std::string& id) = 0;
	
	virtual bool exists(const std::string& id) const = 0;

	virtual uint64_t get_file_size(const std::filesystem::path& file_path) const = 0;
};

} // namespace venturi::core