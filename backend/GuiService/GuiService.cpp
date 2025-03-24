#include <glog/logging.h>

#include <nlohmann/json.hpp>

#include "GuiService.hpp"
#include "crow.h"

namespace GuiService {

void GuiService::start() {
  crow::SimpleApp app;

  CROW_ROUTE(app, "/hello")
  ([]() {
    crow::response res("Hello world from backend");
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Content-Type", "application/json");
    return res;
  });

  CROW_ROUTE(app, "/wallet")
  ([&stockMarketService_ = stockMarketService_]() {
    const std::string accountResponse = stockMarketService_.getAccountData();
    const auto accountJson = nlohmann::json::parse(accountResponse);
    crow::response res(accountJson.dump());

    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Content-Type", "application/json");
    return res;
  });

  LOG(INFO) << "Starting service";
  app.port(5000).run();
}

}  // namespace GuiService