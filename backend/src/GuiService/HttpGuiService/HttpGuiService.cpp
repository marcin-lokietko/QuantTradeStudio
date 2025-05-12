#include <glog/logging.h>

#include <nlohmann/json.hpp>

#include "GuiService/HttpGuiService/Conversion/AssetSymbols.hpp"
#include "GuiService/HttpGuiService/Conversion/Assets.hpp"
#include "GuiService/HttpGuiService/Conversion/AvailableBaseAssetsRequest.hpp"
#include "GuiService/HttpGuiService/Conversion/AvailableQuoteAssets.hpp"
#include "GuiService/HttpGuiService/Conversion/AvailableQuoteAssetsRequest.hpp"
#include "GuiService/HttpGuiService/Conversion/BotConfig.hpp"
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
    LOG(INFO) << "/assets endpoint called";

    crow::response res(Conversion::toJson(apiGateway_.getOwnedAssets()).dump());
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Content-Type", "application/json");

    LOG(INFO) << "/assets endpoint response: " << res.body;
    return res;
  });

  CROW_ROUTE(app, "/openOrders")
  ([&apiGateway_ = apiGateway_]() {
    LOG(INFO) << "/openOrders endpoint called";

    crow::response res(Conversion::toJson(apiGateway_.getOpenOrders()).dump());
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Content-Type", "application/json");

    LOG(INFO) << "/openOrders endpoint response: " << res.body;
    return res;
  });

  // baseAsset=BTC ->
  // [{"quoteAsset":"USDT", "baseAssetUnitPrice":80000.0}, {{"quoteAsset":"ETH", "baseAssetUnitPrice":50.0}}]
  CROW_ROUTE(app, "/availableQuoteAssets")
  ([&apiGateway_ = apiGateway_](const crow::request& req) {
    LOG(INFO) << "/availableQuoteAssets endpoint called";

    crow::response res{};

    std::optional<AvailableQuoteAssetsRequest> availableQuoteAssetsRequest;
    Conversion::fromQueryParams(crow::query_string(req.url_params), availableQuoteAssetsRequest);

    if (!availableQuoteAssetsRequest) {
      res.code = 500;
    } else {
      res.body = Conversion::toJson(apiGateway_.getAvailableQuoteAssets(availableQuoteAssetsRequest->baseAsset)).dump();
    }

    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Content-Type", "application/json");

    LOG(INFO) << "/availableQuoteAssets endpoint response: " << res.body;
    return res;
  });

  // "" -> [{"baseAsset":"BTC"}, {{"baseAsset":"USDT"}]
  // quoteAsset=BTC -> [{"baseAsset":"BTC"}, {{"baseAsset":"USDT"}]
  CROW_ROUTE(app, "/availableBaseAssets")
  ([&apiGateway_ = apiGateway_](const crow::request& req) {
    LOG(INFO) << "/availableBaseAssets endpoint called";

    crow::response res{};

    AvailableBaseAssetsRequest availableBaseAssetsRequest;
    Conversion::fromQueryParams(crow::query_string(req.url_params), availableBaseAssetsRequest);

    res.body = Conversion::toJson(apiGateway_.getAvailableBaseAssets(availableBaseAssetsRequest.quoteAsset)).dump();
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Content-Type", "application/json");

    LOG(INFO) << "/availableBaseAssets endpoint response: " << res.body;
    return res;
  });

  CROW_ROUTE(app, "/quoteAssetsSuitableForRebalancing")
  ([&apiGateway_ = apiGateway_]() {
    LOG(INFO) << "/quoteAssetsSuitableForRebalancing endpoint called";

    crow::response res{};
    res.body = Conversion::toJson(apiGateway_.getQuoteAssetsSuitableForRebalancing()).dump();

    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Content-Type", "application/json");

    LOG(INFO) << "/quoteAssetsSuitableForRebalancing endpoint response: " << res.body;
    return res;
  });

  CROW_ROUTE(app, "/makeOrder").methods("POST"_method)([&apiGateway_ = apiGateway_](const crow::request& req) {
    LOG(INFO) << "/makeOrder endpoint called";
    OrderRequest orderRequest;
    Conversion::fromJson(nlohmann::json::parse(req.body), orderRequest);

    const auto result = apiGateway_.makeOrder(orderRequest.selectedBaseAsset, orderRequest.selectedQuoteAsset,
                                              orderRequest.orderSide, orderRequest.baseAssetAmount);

    crow::response res{"result"};
    if (result != ApiGateway::OrderResult::Success) {
      res.code = 500;
    }
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Content-Type", "application/json");

    LOG(INFO) << "/makeOrder endpoint response: " << res.body;
    return res;
  });

  CROW_ROUTE(app, "/startBot").methods("POST"_method)([&apiGateway_ = apiGateway_](const crow::request& req) {
    LOG(INFO) << "/startBot endpoint called";

    ApiGateway::BotConfig botConfig;
    Conversion::fromJson(nlohmann::json::parse(req.body), botConfig);

    const auto result = apiGateway_.startBot(botConfig);

    crow::response res{"result"};
    if (result != ApiGateway::StartBotResult::Success) {
      res.code = 500;
    }
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Content-Type", "application/json");

    LOG(INFO) << "/startBot endpoint response: " << res.body;
    return res;
  });

  LOG(INFO) << "Starting service";
  app.port(5000).run();
}

}  // namespace GuiService::HttpGuiService