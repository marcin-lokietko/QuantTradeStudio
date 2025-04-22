#pragma once

#include <string>

#include "Utils/StrongType.hpp"

namespace Http {

DEFINE_STRONG_TYPE(HttpStatusCode, int64_t);
DEFINE_STRONG_TYPE(HttpBody, std::string);

class Http {
  struct Response {
    HttpStatusCode statusCode{};
    HttpBody body{};
  };

 public:
  std::string get(const std::string& url, const std::string& header);
  Response post(const std::string& url, const std::string& header);
  Response del(const std::string& url, const std::string& header);
};

}  // namespace Http