#pragma once

#include "ApiGateway/IApiGateway.hpp"
#include "GuiService/IGuiService.hpp"

namespace GuiService::HttpGuiService {

class HttpGuiService : public IGuiService {
 public:
  HttpGuiService(const ApiGateway::IApiGateway& apiGateway) : apiGateway_(apiGateway){};

  void start() override;

 private:
  const ApiGateway::IApiGateway& apiGateway_;
};

}  // namespace GuiService::HttpGuiService