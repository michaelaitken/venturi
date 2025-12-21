#include "HttpSession.hpp"
#include "../../../app/Logger.hpp"

#include <boost/beast/version.hpp>
#include <sstream>
#include <iomanip>

namespace venturi::adapters {

HttpSession::HttpSession(
  tcp::socket                           socket,
  std::shared_ptr<core::MediaService>   media_service,
  const Config&                         config
) 
  : stream_(std::move(socket))
  , media_service_(std::move(media_service))
  , config_(config)
{}

void HttpSession::run() {
  stream_.expires_after(std::chrono::seconds(30));
  this->do_read();
}

void HttpSession::do_read() {
  request_ = {}; // reset
  
  http::async_read(
    stream_,
    buffer_,
    request_,
    beast::bind_front_handler(
      &HttpSession::on_read,
      this->shared_from_this()  // Kepp the object alive inside handler
    )
  );
}

void HttpSession::on_read(
  beast::error_code ec,
  std::size_t       bytes_transferred
) {
  boost::ignore_unused(bytes_transferred);
  
  if (ec == http::error::end_of_stream) {
    return this->do_close();
  }
  
  if (ec) {
    LOG_ERROR("Read error: ", ec.message());
    return;
  }
  
  this->handle_request();
}

void HttpSession::handle_request() {
  LOG_INFO(request_.method_string(), " ", request_.target());
  
  std::string target{ std::string(request_.target()) };

  if (request_.method() == http::verb::get) {
    if (target == "/api/media") {
      return this->handle_list_media();
    }

    else if (target.starts_with("/api/media/")) {
      std::string media_id{ target.substr(11) };

      // Removes query parameters
      if (std::size_t pos{ media_id.find('?') }; pos != std::string::npos) {
        media_id = media_id.substr(0, pos);
      }

      return this->handle_get_media(media_id);
    }

    else if (target == "/api/scan") {
      return this->handle_scan();
    }
  }
  
  this->send_error(http::status::not_found, "Endpoint not found.");
}

void HttpSession::handle_get_media(const std::string& media_id) {
  auto media{ media_service_->get_media(media_id) };
  if (!media) {
    return this->send_error(http::status::not_found, "Media not found.");
  }
  
  beast::error_code ec;
  http::file_body::value_type body;
  body.open(media->file_path.string().c_str(), beast::file_mode::scan, ec);

  if (ec) {
    LOG_ERROR("Failed to open file: ", media->file_path.string());
    return this->send_error(http::status::internal_server_error, "File access error");
  }

  uint64_t file_size = body.size();

  auto response = std::make_shared<http::response<http::file_body>>(
    std::piecewise_construct,
    std::make_tuple(std::move(body)),
    std::make_tuple(http::status::ok, request_.version())
  );
  
  response->set(http::field::server, "Venturi/1.0");
  response->set(http::field::content_type, media->mime_type);
  response->set(http::field::accept_ranges, "bytes");
  response->keep_alive(request_.keep_alive());

  auto range_header = request_.find(http::field::range);
  if (range_header != request_.end()) {
    auto range = media_service_->parse_range_header(
      std::string(range_header->value()), file_size
    );

    if (range) {
      response->result(http::status::partial_content);
      
      response->body().seek(range->start, ec); 
      if(ec) return send_error(http::status::internal_server_error, "Seek failed");

      // (TODO): [BUG] Beast's file_body seems to always read to EOF.
      //         May need to implement platform specific disk-to-socket support.
      response->content_length(range->length());

      std::ostringstream range_str;
      range_str << "bytes " << range->start << "-" << range->end << "/" << range->total_size;
      response->set(http::field::content_range, range_str.str());
    } else {
      return send_error(http::status::range_not_satisfiable, "Invalid Range");
    }
  } else {
    // Full file
    response->content_length(file_size);
  }


  http::async_write(
    stream_,
    *response,
    [self = shared_from_this(), response](beast::error_code ec, std::size_t bytes_transferred) {
      if (ec) {
        if(ec != asio::error::connection_reset)
          LOG_ERROR("Stream error: ", ec.message());
        return; 
      }
      
      if (self->request_.keep_alive()) {
        self->do_read();
      } else {
        self->do_close();
      }
    }
  );
}

void HttpSession::handle_list_media() {
  auto media_list = media_service_->list_all_media();
  
  // (TODO): Replace with a proper JSON library
  std::ostringstream json;
  json << "{\"media\":[";
  
  for (size_t i = 0; i < media_list.size(); ++i) {
    const auto& m = media_list[i];
    if (i > 0) json << ",";
    
    json << "{"
          << "\"id\":\"" << m.id << "\","
          << "\"path\":\"" << m.file_path.string() << "\","
          << "\"mime\":" << m.mime_type << ","
          << "}";
  }
  
  json << "]}";
  send_json(json.str());
}

void HttpSession::handle_scan() {
  size_t count = media_service_->scan_media_directory(config_.media_root);

  std::ostringstream json;
  json << "{\"scanned\":" << count << "}";
  send_json(json.str());
}

void HttpSession::send_json(const std::string& json) {
  auto response = std::make_shared<http::response<http::string_body>>(
    http::status::ok, request_.version()
  );

  response->set(http::field::server, "Venturi/1.0");
  response->set(http::field::content_type, "application/json");
  response->keep_alive(request_.keep_alive());
  response->body() = json;
  response->prepare_payload();
  
  http::async_write(
    stream_,
    *response,
    [self = shared_from_this(), response](beast::error_code ec, std::size_t) {
      if (ec) return;
      if (self->request_.keep_alive()) self->do_read();
      else self->do_close();
    }
  );
}

void HttpSession::send_error(
  http::status status,
  const std::string& message
) {
  auto response = std::make_shared<http::response<http::string_body>>(
    status, request_.version()
  );

  response->set(http::field::server, "Venturi/1.0");
  response->set(http::field::content_type, "text/plain");
  response->keep_alive(request_.keep_alive());
  response->body() = message;
  response->prepare_payload();
  
  http::async_write(
    stream_,
    *response,
    [self = shared_from_this(), response](beast::error_code ec, std::size_t) {
      if (ec) return;
      if (self->request_.keep_alive()) self->do_read();
      else self->do_close();
    }
  );
}

void HttpSession::do_close() {
  beast::error_code ec;
  stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
}

} // namespace venturi::adapters