#pragma once

#include "Http/IHttp.hpp"

namespace Http {

class Http : public IHttp {
 public:
  std::string get(const std::string& url, const std::string& header) const override;
  Response post(const std::string& url, const std::string& header) const override;
  Response del(const std::string& url, const std::string& header) const override;
};

}  // namespace Http