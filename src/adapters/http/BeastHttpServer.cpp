#include "BeastHttpServer.hpp"
#include "HttpSession.hpp"

#include "../../../app/Logger.hpp"

namespace venturi::adapters {

BeastHttpServer::BeastHttpServer(
  std::shared_ptr<core::MediaService>   media_service,
  const Config&                         config
) 
  : media_service_(std::move(media_service))
  , config_(config)
{}

BeastHttpServer::~BeastHttpServer() {
  this->stop();
}

void BeastHttpServer::start(
  const std::string&  host,
  uint16_t            port,
  uint32_t            thread_count
) {
  if (running_.exchange(true)) {
    LOG_WARN("Server already running.");
    return;
  }
  
  try {
    asio::ip::address const address{ asio::ip::make_address(host) };
    tcp::endpoint endpoint{ address, port };
    
    acceptor_ = std::make_unique<tcp::acceptor>(ioc_);
    acceptor_->open(endpoint.protocol());
    acceptor_->set_option(asio::socket_base::reuse_address(true));
    acceptor_->bind(endpoint);
    acceptor_->listen(asio::socket_base::max_listen_connections);
    
    LOG_INFO("Server listening on ", host, ":", port);
    
    this->do_accept();
    
    threads_.reserve(thread_count);
    for (uint32_t i{ 0 }; i < thread_count; ++i) {
      // Each thread will process async events by calling ioc_.run().
      threads_.emplace_back([this] {
        ioc_.run();
      });
    }
    
    LOG_INFO("Server started with ", thread_count, " threads");
      
  } catch (const std::exception& ex) {
    LOG_ERROR("Failed to start server: ", ex.what());
    running_ = false;
    throw;
  }
}

void BeastHttpServer::stop() {
  if (!running_.exchange(false)) {
    return;
  }
  
  LOG_INFO("Stopping server...");
  
  if (acceptor_) {
    beast::error_code ec;
    acceptor_->close(ec);
  }
  
  ioc_.stop();
  
  for (auto& thread : threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  
  threads_.clear();
  LOG_INFO("Server stopped successfully.");
}

bool BeastHttpServer::is_running() const { return running_; }

void BeastHttpServer::do_accept() {
  acceptor_->async_accept(
    asio::make_strand(ioc_),
    beast::bind_front_handler(
      &BeastHttpServer::on_accept,
      this
    )
  );
}

void BeastHttpServer::on_accept(
  beast::error_code ec,
  tcp::socket       socket
) {
  if (ec) {
    if (ec != asio::error::operation_aborted) {
      LOG_ERROR("Accept error: ", ec.message());
    }
  } else {
    // Create and run session
    std::make_shared<HttpSession>(
      std::move(socket),
      media_service_,
      config_
    )->run();
  }
  
  // Accept next connection
  if (running_) {
    this->do_accept();
  }
}

} // namespace venturi::adapters