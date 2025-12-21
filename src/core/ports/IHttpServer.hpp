#pragma once
#include <cstdint>
#include <string>
#include <functional>

namespace venturi::core {

class IHttpServer {
public:
  virtual ~IHttpServer() = default;
  
  virtual void start(
    const std::string& host,
    uint16_t port,
    uint32_t thread_count
  ) = 0;
  
  virtual void stop() = 0;
  virtual bool is_running() const = 0;
};

} // namespace venturi::core