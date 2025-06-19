#pragma once

#include <string>

#include "Utils/StrongType.hpp"

namespace Http {

DEFINE_STRONG_TYPE(HttpStatusCode, int64_t);
DEFINE_STRONG_TYPE(HttpBody, std::string);

struct Response {
  HttpStatusCode statusCode{};
  HttpBody body{};
};

class IHttp {
 public:
  virtual ~IHttp() = default;

  virtual std::string get(const std::string& url, const std::string& header) const = 0;
  virtual Response post(const std::string& url, const std::string& header) const = 0;
  virtual Response del(const std::string& url, const std::string& header) const = 0;
};

}  // namespace Http