#pragma once
#include "../core/services/MediaService.hpp"
#include "../core/ports/IMediaRepository.hpp"
#include "../core/ports/IHttpServer.hpp"
#include "Config.hpp"
#include <memory>

namespace venturi {

class Application {
public:
  explicit Application(const Config& config);
  
  std::shared_ptr<core::MediaService> get_media_service() const {
    return media_service_;
  }
  
  std::shared_ptr<core::IHttpServer> get_http_server() const {
    return http_server_;
  }
  
  void start_services();
  void stop_services();

private:
  // (TODO): Explore C++20 Concepts to replace with Static Polymorphism
  std::shared_ptr<core::IMediaRepository> media_repository_;
  std::shared_ptr<core::IHttpServer> http_server_;
  std::shared_ptr<core::MediaService> media_service_;
  const Config& config_;
};

} // namespace venturi