#pragma once
#include "../../core/services/MediaService.hpp"
#include "../../../app/Config.hpp"
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>

namespace venturi::adapters {

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class HttpSession : public std::enable_shared_from_this<HttpSession> {
public:
  HttpSession(
    tcp::socket                           socket,
    std::shared_ptr<core::MediaService>   media_service,
    const Config&                         config
  );
  
  void run();

private:
  void do_read();
  
  // Called when an HTTP request has been read (or there was an error).
  void on_read(beast::error_code ec, std::size_t bytes_transferred);
  
  // Decide which endpoint method to call based on the request.
  void handle_request();


  void handle_get_media(const std::string& media_id);
  void handle_list_media();
  void handle_scan();
  
  void send_error(http::status status, const std::string& message);
  void send_json(const std::string& json);
  
  // Gracefully close the connection.
  void do_close();
  
  beast::tcp_stream stream_;
  beast::flat_buffer buffer_;
  http::request<http::string_body> request_;
  
  std::shared_ptr<core::MediaService> media_service_;
  const Config& config_;
};

} // namespace venturi::adapters