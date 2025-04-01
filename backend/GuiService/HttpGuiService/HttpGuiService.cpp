#include <glog/logging.h>

#include <nlohmann/json.hpp>

#include "GuiService/HttpGuiService/Conversion/Assets.hpp"
#include "HttpGuiService.hpp"
#include "crow.h"

namespace GuiService::HttpGuiService {

void HttpGuiService::start() {
  crow::SimpleApp app;

  CROW_ROUTE(app, "/hello")
  ([]() {
    crow::response res("Hello world from backend");
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Content-Type", "application/json");
    return res;
  });

  CROW_ROUTE(app, "/assets")
  ([&apiGateway_ = apiGateway_]() {
    crow::response res(Conversion::toJson(apiGateway_.getAssets()).dump());
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Content-Type", "application/json");
    return res;
  });

  LOG(INFO) << "Starting service";
  app.port(5000).run();
}

}  // namespace GuiService::HttpGuiService