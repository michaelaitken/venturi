#pragma once
#include "../../core/ports/IHttpServer.hpp"
#include "../../core/services/MediaService.hpp"
#include "../../../app/Config.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <memory>
#include <vector>
#include <thread>

namespace venturi::adapters {

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class BeastHttpServer : public core::IHttpServer {
public:
  BeastHttpServer(
    std::shared_ptr<core::MediaService>   media_service,
    const Config&                         config
  );
  
  ~BeastHttpServer() override;
  
  void start(
    const std::string&  host,
    uint16_t            port,
    uint32_t            thread_count
  ) override;
  
  void stop() override;
  bool is_running() const override;

private:
  void do_accept();
  void on_accept(beast::error_code ec, tcp::socket socket);

  std::shared_ptr<core::MediaService> media_service_;
  const Config& config_;
  asio::io_context ioc_;
  std::unique_ptr<tcp::acceptor> acceptor_;
  std::vector<std::thread> threads_;
  std::atomic<bool> running_{ false };
};

} // namespace venturi::adapters