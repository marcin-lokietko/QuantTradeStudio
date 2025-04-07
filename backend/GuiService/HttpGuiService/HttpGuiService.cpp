#include <glog/logging.h>

#include <nlohmann/json.hpp>

#include "GuiService/HttpGuiService/Conversion/Assets.hpp"
#include "GuiService/HttpGuiService/Conversion/OrderRequest.hpp"
#include "GuiService/HttpGuiService/Conversion/Orders.hpp"
#include "GuiService/OrderRequest.hpp"
#include "HttpGuiService.hpp"
#include "crow.h"
#include "crow/middlewares/cors.h"

namespace GuiService::HttpGuiService {

void HttpGuiService::start() {
  // Configure CORS
  crow::App<crow::CORSHandler> app;
  auto& cors = app.get_middleware<crow::CORSHandler>();

  cors.global()
      .methods("POST"_method, "GET"_method, "OPTIONS"_method)
      .origin("*")           // TODO - restrict to frontend’s domain
      .allow_credentials();  // Allow credentials like cookies or Authorization header

  // Setup endpoints
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

  CROW_ROUTE(app, "/openOrders")
  ([&apiGateway_ = apiGateway_]() {
    crow::response res(Conversion::toJson(apiGateway_.getOpenOrders()).dump());
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Content-Type", "application/json");
    return res;
  });

  CROW_ROUTE(app, "/makeOrder").methods("POST"_method)([&apiGateway_ = apiGateway_](const crow::request& req) {
    OrderRequest orderRequest;
    Conversion::fromJson(nlohmann::json::parse(req.body), orderRequest);

    const auto result =
        apiGateway_.makeOrder(orderRequest.assetToBuy, orderRequest.assetToSpend, orderRequest.quantityToBuy);

    crow::response res{"result"};
    if (result != ApiGateway::OrderResult::Success) {
      res.code = 500;
    }
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Content-Type", "application/json");
    return res;
  });

  LOG(INFO) << "Starting service";
  app.port(5000).run();
}

}  // namespace GuiService::HttpGuiService