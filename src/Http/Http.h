#pragma once

#include <string>

namespace Http {

class Http {
 public:
  std::string get(const std::string& url, const std::string& header);
  std::string post(const std::string& url, const std::string& header);
};

}  // namespace Http