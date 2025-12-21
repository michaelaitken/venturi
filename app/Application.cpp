#include "Application.hpp"
#include "../adapters/storage/FileSystemRepository.hpp"
#include "../adapters/http/BeastHttpServer.hpp"
#include "Logger.hpp"

namespace venturi {

Application::Application(const Config& config)
  : config_(config)
{
  LOG_INFO("Initializing application...");
  
  media_repository_ = std::make_shared<adapters::FileSystemRepository>(
    config_.media_root,
    config_.transcode_output
  );
  
  media_service_ = std::make_shared<core::MediaService>(
    media_repository_
  );

  http_server_ = std::make_shared<adapters::BeastHttpServer>(
    media_service_,
    config_
  );
  
  LOG_INFO("Application initialized.");
}

void Application::start_services() {
  LOG_INFO("Starting services...");
  
  LOG_INFO("Performing initial media scan...");
  size_t count = media_service_->scan_media_directory(config_.media_root);
  LOG_INFO("Found ", count, " media files");
  
  http_server_->start(config_.host, config_.port, config_.thread_count);
}

void Application::stop_services() {
  LOG_INFO("Stopping services...");
  
  http_server_->stop();
  
  LOG_INFO("All services stopped");
}

} // namespace venturi