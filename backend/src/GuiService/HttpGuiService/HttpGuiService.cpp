#include "HttpGuiService.hpp"

#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>

#include "GuiService/HealthRequest.hpp"
#include "GuiService/HttpGuiService/Conversion/AssetSymbols.hpp"
#include "GuiService/HttpGuiService/Conversion/Assets.hpp"
#include "GuiService/HttpGuiService/Conversion/AvailableBaseAssetsRequest.hpp"
#include "GuiService/HttpGuiService/Conversion/AvailableQuoteAssets.hpp"
#include "GuiService/HttpGuiService/Conversion/AvailableQuoteAssetsRequest.hpp"
#include "GuiService/HttpGuiService/Conversion/BacktestConfig.hpp"
#include "GuiService/HttpGuiService/Conversion/BacktestResults.hpp"
#include "GuiService/HttpGuiService/Conversion/BotConfig.hpp"
#include "GuiService/HttpGuiService/Conversion/HealthRequest.hpp"
#include "GuiService/HttpGuiService/Conversion/OrderRequest.hpp"
#include "GuiService/HttpGuiService/Conversion/Orders.hpp"
#include "GuiService/OrderRequest.hpp"
#include "crow.h"
#include "crow/middlewares/cors.h"

namespace GuiService::HttpGuiService {

namespace {
crow::response buildEmptyResponse(std::string body = "") {
  crow::response res{std::move(body)};
  res.add_header("Access-Control-Allow-Origin", "*");
  res.add_header("Content-Type", "application/json");
  return res;
}
}  // namespace

void HttpGuiService::start() {
  crow::App<crow::CORSHandler> app;
  auto& cors = app.get_middleware<crow::CORSHandler>();

  cors.global()
      .methods("POST"_method, "GET"_method, "OPTIONS"_method)
      .origin("*")
      .allow_credentials();  // Allow credentials like cookies or Authorization header

  // Setup endpoints
  CROW_ROUTE(app, "/health")
  ([](const crow::request& req) {
    SPDLOG_INFO("/health endpoint called");

    std::optional<HealthRequest> healthRequest;
    Conversion::fromQueryParams(crow::query_string(req.url_params), healthRequest);

    if (healthRequest) {
      SPDLOG_INFO("/health endpoint request message: {}", healthRequest->message);
    }

    return buildEmptyResponse();
  });

  CROW_ROUTE(app, "/assets")
  ([&apiGateway_ = apiGateway_]() {
    SPDLOG_INFO("/assets endpoint called");

    auto res = buildEmptyResponse(Conversion::toJson(apiGateway_.getOwnedAssets()).dump());
    SPDLOG_INFO("/assets endpoint response: {}", res.body);
    return res;
  });

  CROW_ROUTE(app, "/openOrders")
  ([&apiGateway_ = apiGateway_]() {
    SPDLOG_INFO("/openOrders endpoint called");

    auto res = buildEmptyResponse(Conversion::toJson(apiGateway_.getOpenOrders()).dump());

    SPDLOG_INFO("/openOrders endpoint response: {}", res.body);
    return res;
  });

  // baseAsset=BTC ->
  // [{"quoteAsset":"USDT", "baseAssetUnitPrice":80000.0}, {{"quoteAsset":"ETH", "baseAssetUnitPrice":50.0}}]
  CROW_ROUTE(app, "/availableQuoteAssets")
  ([&apiGateway_ = apiGateway_](const crow::request& req) {
    SPDLOG_INFO("/availableQuoteAssets endpoint called");

    auto res = buildEmptyResponse();
    std::optional<AvailableQuoteAssetsRequest> availableQuoteAssetsRequest;
    Conversion::fromQueryParams(crow::query_string(req.url_params), availableQuoteAssetsRequest);

    if (!availableQuoteAssetsRequest) {
      res.code = 500;
    } else {
      res.body = Conversion::toJson(apiGateway_.getAvailableQuoteAssets(availableQuoteAssetsRequest->baseAsset)).dump();
    }

    SPDLOG_INFO("/availableQuoteAssets endpoint response: {}", res.body);
    return res;
  });

  // "" -> [{"baseAsset":"BTC"}, {{"baseAsset":"USDT"}]
  // quoteAsset=BTC -> [{"baseAsset":"BTC"}, {{"baseAsset":"USDT"}]
  CROW_ROUTE(app, "/availableBaseAssets")
  ([&apiGateway_ = apiGateway_](const crow::request& req) {
    SPDLOG_INFO("/availableBaseAssets endpoint called");

    AvailableBaseAssetsRequest availableBaseAssetsRequest;
    Conversion::fromQueryParams(crow::query_string(req.url_params), availableBaseAssetsRequest);

    auto res = buildEmptyResponse(
        Conversion::toJson(apiGateway_.getAvailableBaseAssets(availableBaseAssetsRequest.quoteAsset)).dump());

    SPDLOG_INFO("/availableBaseAssets endpoint response: {}", res.body);
    return res;
  });

  CROW_ROUTE(app, "/quoteAssetsSuitableForBots")
  ([&apiGateway_ = apiGateway_]() {
    SPDLOG_INFO("/quoteAssetsSuitableForBots endpoint called");

    auto res = buildEmptyResponse(Conversion::toJson(apiGateway_.getQuoteAssetsSuitableForBots()).dump());

    SPDLOG_INFO("/quoteAssetsSuitableForBots endpoint response: {}", res.body);
    return res;
  });

  CROW_ROUTE(app, "/makeOrder").methods("POST"_method)([&apiGateway_ = apiGateway_](const crow::request& req) {
    SPDLOG_INFO("/makeOrder endpoint called");
    OrderRequest orderRequest;

    try {
      Conversion::fromJson(nlohmann::json::parse(req.body), orderRequest);
    } catch (const nlohmann::json::exception& e) {
      SPDLOG_CRITICAL("/makeOrder FAILED; could not deserialize request; error: {}", e.what());
      auto res = buildEmptyResponse();
      res.code = 500;
      return res;
    }

    const auto result = apiGateway_.makeOrder(orderRequest.selectedBaseAsset, orderRequest.selectedQuoteAsset,
                                              orderRequest.orderSide, orderRequest.baseAssetAmount);

    auto res = buildEmptyResponse();
    if (result != ApiGateway::OrderResult::Success) {
      res.code = 500;
    }

    SPDLOG_INFO("/makeOrder endpoint response: {}", res.body);
    return res;
  });

  CROW_ROUTE(app, "/startBot").methods("POST"_method)([&apiGateway_ = apiGateway_](const crow::request& req) {
    SPDLOG_INFO("/startBot endpoint called");

    ApiGateway::BotConfig botConfig;

    try {
      Conversion::fromJson(nlohmann::json::parse(req.body), botConfig);
    } catch (const nlohmann::json::exception& e) {
      SPDLOG_CRITICAL("/startBot FAILED; could not deserialize request; error: {}", e.what());
      auto res = buildEmptyResponse();
      res.code = 500;
      return res;
    }

    const auto result = apiGateway_.startBot(botConfig);

    auto res = buildEmptyResponse();
    if (result != ApiGateway::StartBotResult::Success) {
      res.code = 500;
    }

    SPDLOG_INFO("/startBot endpoint response: {}; status code: {}", res.body, res.code);
    return res;
  });

  CROW_ROUTE(app, "/stopAllBots").methods("POST"_method)([&apiGateway_ = apiGateway_]() {
    SPDLOG_INFO("/stopAllBots endpoint called");

    const auto result = apiGateway_.stopAllBots();

    auto res = buildEmptyResponse();
    if (result != ApiGateway::StopAllBotsResult::Success) {
      res.code = 500;
    }

    SPDLOG_INFO("/stopAllBots endpoint response: {}; status code: {}", res.body, res.code);
    return res;
  });

  CROW_ROUTE(app, "/testBot").methods("POST"_method)([&apiGateway_ = apiGateway_](const crow::request& req) {
    SPDLOG_INFO("/testBot endpoint called");

    ApiGateway::BotConfig botConfig;
    ApiGateway::BacktestConfig backtestConfig;

    try {
      Conversion::fromJson(nlohmann::json::parse(req.body), botConfig);
      Conversion::fromJson(nlohmann::json::parse(req.body), backtestConfig);
    } catch (const nlohmann::json::exception& e) {
      SPDLOG_CRITICAL("/testBot FAILED; could not deserialize request; error: {}", e.what());
      auto res = buildEmptyResponse();
      res.code = 500;
      return res;
    }

    auto res = buildEmptyResponse(Conversion::toJson(apiGateway_.testBot(botConfig, backtestConfig)).dump());
    SPDLOG_INFO("/testBot endpoint response: {}", res.body);
    return res;
  });

  SPDLOG_INFO("Starting service");
  app.port(5000).run();
}

}  // namespace GuiService::HttpGuiService
