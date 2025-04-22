#include <glog/logging.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <ranges>

#include "BinanceService.hpp"
#include "Http/Http.hpp"
#include "MarketService/Binance/Conversion/AssetPrices.hpp"
#include "MarketService/Binance/Conversion/Assets.hpp"
#include "MarketService/Binance/Conversion/Orders.hpp"
#include "MarketService/Binance/Conversion/TradingPairs.hpp"

namespace MarketService::Binance {

namespace {
std::string getTimeSinceEpoch() { return std::to_string(time(nullptr) * 1000); }
constexpr Http::HttpStatusCode statusCodeOk{200};
constexpr Http::HttpStatusCode statusCodeRequestRateLimitBroken{429};
}  // namespace

std::string BinanceService::getServerTime() {
  const std::string timeUrl = binanceTestnetBaseUrl + "/api/v3/time";
  return Http::Http().get(timeUrl, "");
}

ApiGateway::Price BinanceService::getPrice(const ApiGateway::TradingPairSymbol& tradingPairSymbol) {
  std::string url = binanceTestnetBaseUrl + "/api/v3/ticker/price?symbol=" + tradingPairSymbol.val_;
  const auto response = Http::Http().get(url, "");

  const auto jsonResponse = nlohmann::json::parse(response);
  if (jsonResponse.contains("price")) {
    return ApiGateway::Price{jsonResponse["price"]};
  } else {
    LOG(ERROR) << "Failed to retrieve price.";
  }
  return ApiGateway::Price{""};
}

AssetPrices BinanceService::getPrices(const std::vector<ApiGateway::TradingPairSymbol>& tradingPairSymbols) const {
  std::string symbolsString = "[";
  for (const auto& singleSymbol : tradingPairSymbols) {
    if (symbolsString.back() != '[') {
      symbolsString += ",";
    }
    symbolsString += '"';
    symbolsString += singleSymbol.val_;
    symbolsString += '"';
  }
  symbolsString += "]";
  const std::string url = binanceTestnetBaseUrl + "/api/v3/ticker/price?symbols=" + symbolsString;
  const auto response = Http::Http().get(url, "");

  AssetPrices assetPrices;
  Conversion::fromJson(nlohmann::json::parse(response), assetPrices);
  return assetPrices;
}

// e.g. symbol="BTCUSDT", interval="1h"
MarketService::KlineSequence BinanceService::getKlines(const std::string& symbol, const std::string& interval) {
  const std::string klinesUrl =
      binanceTestnetBaseUrl + "/api/v3/klines?symbol=" + symbol + "&interval=" + interval + "&limit=1000";
  const auto klinesString = Http::Http().get(klinesUrl, "");
  const auto klinesJson = nlohmann::json::parse(klinesString);

  MarketService::KlineSequence sequence;
  sequence.reserve(klinesJson.size());

  for (const auto& kline : klinesJson) {
    std::string closePrice = kline[4].get<std::string>();
    uint64_t closeTime = kline[6].get<uint64_t>();
    sequence.push_back({.closeTime = closeTime, .closePrice = closePrice});
  }

  return sequence;
}

Assets BinanceService::getOwnedAssets() const {
  const auto accountUrl = getAccountUrl();
  const auto accountString = Http::Http().get(accountUrl, "X-MBX-APIKEY: " + encryption.getApiKey());
  const auto accountJson = nlohmann::json::parse(accountString);

  Assets assets;
  MarketService::Binance::Conversion::fromJson(accountJson.at("balances"), assets);
  return assets;
}

ApiGateway::OrderResult BinanceService::makeOrder(const ApiGateway::TradingPairSymbol& symbol,
                                                  const ApiGateway::OrderSide& orderSide,
                                                  const ApiGateway::AssetQuantity& quantity,
                                                  const ApiGateway::Price& price) {
  std::string orderSideString = toString(orderSide);
  std::ranges::transform(orderSideString, orderSideString.begin(), [](unsigned char c) { return std::toupper(c); });

  const std::string queryString = "symbol=" + symbol.val_ + "&side=" + orderSideString +
                                  "&type=LIMIT&timeInForce=GTC&quantity=" + quantity.val_ + "&price=" + price.val_ +
                                  "&recvWindow=5000&timestamp=" + getTimeSinceEpoch();
  const auto orderUrl = getOrderUrl(queryString);
  const auto response = Http::Http().post(orderUrl, "X-MBX-APIKEY: " + encryption.getApiKey());

  if (response.statusCode != statusCodeOk) {
    LOG(WARNING) << "makeOrder returned NOK. Msg=" << response.body.val_;
    return ApiGateway::OrderResult::Failure;
  }
  return ApiGateway::OrderResult::Success;
}

ApiGateway::OrderResult BinanceService::makeMarketTypeOrderWithQuoteQuantity(
    const ApiGateway::TradingPairSymbol& symbol, const ApiGateway::OrderSide& orderSide,
    const ApiGateway::AssetQuantity& quoteQuantity) const {
  std::string orderSideString = toString(orderSide);
  LOG(INFO) << "makeMarketTypeOrderWithQuoteQuantity symbol=" << symbol.val_ << " orderSide=" << orderSideString
            << " quoteQuantity=" << quoteQuantity.val_;

  std::ranges::transform(orderSideString, orderSideString.begin(), [](unsigned char c) { return std::toupper(c); });

  const std::string queryString = "symbol=" + symbol.val_ + "&side=" + orderSideString +
                                  "&type=MARKET&quoteOrderQty=" + quoteQuantity.val_ +
                                  "&recvWindow=5000&timestamp=" + getTimeSinceEpoch();
  const auto orderUrl = getOrderUrl(queryString);
  const auto response = Http::Http().post(orderUrl, "X-MBX-APIKEY: " + encryption.getApiKey());

  if (response.statusCode != statusCodeOk) {
    LOG(WARNING) << "makeMarketTypeOrderWithQuoteQuantity returned NOK. Msg=" << response.body.val_;
    return ApiGateway::OrderResult::Failure;
  }
  return ApiGateway::OrderResult::Success;
}

TradingPairs BinanceService::getAllTradingPairs() const {
  const std::string url = binanceTestnetBaseUrl + "/api/v3/exchangeInfo";
  const auto tradingPairsString = Http::Http().get(url, "");

  TradingPairs tradingPairs;
  Conversion::fromJson(nlohmann::json::parse(tradingPairsString), tradingPairs);
  return tradingPairs;
}

TradingPairs BinanceService::getTradingPairsWithQuoteAsset(const ApiGateway::AssetSymbol& quoteAsset) const {
  const auto tradingPairs = getAllTradingPairs();
  auto filteredPairsView =
      tradingPairs | std::views::filter([&quoteAsset](const auto& pair) { return pair.quoteAsset == quoteAsset; });
  return {filteredPairsView.begin(), filteredPairsView.end()};
}

TradingPairs BinanceService::getTradingPairsWithBaseAsset(const ApiGateway::AssetSymbol& baseAsset) const {
  const auto tradingPairs = getAllTradingPairs();
  auto filteredPairsView =
      tradingPairs | std::views::filter([&baseAsset](const auto& pair) { return pair.baseAsset == baseAsset; });
  return {filteredPairsView.begin(), filteredPairsView.end()};
}

std::string BinanceService::getAccountUrl() const {
  const std::string timestamp = "timestamp=" + getTimeSinceEpoch();
  const std::string signature = encryption.generateSignature(timestamp);
  const std::string signedQuery = timestamp + "&signature=" + signature;
  return binanceTestnetBaseUrl + "/api/v3/account?" + signedQuery;
}

std::string BinanceService::getOrderUrl(const std::string& queryString) const {
  const std::string signature = encryption.generateSignature(queryString);
  const std::string signedQuery = queryString + "&signature=" + signature;
  return binanceTestnetBaseUrl + "/api/v3/order?" + signedQuery;
}

ApiGateway::Orders BinanceService::getOpenOrders() const {
  const std::string timestamp = "timestamp=" + getTimeSinceEpoch();
  const std::string signature = encryption.generateSignature(timestamp);
  const std::string signedQuery = timestamp + "&signature=" + signature;
  const std::string url = binanceTestnetBaseUrl + "/api/v3/openOrders" + "?" + signedQuery;

  const auto openOrdersString = Http::Http().get(url, "X-MBX-APIKEY: " + encryption.getApiKey());

  ApiGateway::Orders orders;
  Conversion::fromJson(nlohmann::json::parse(openOrdersString), orders);
  return orders;
}

ApiGateway::OrderResult BinanceService::cancelAllOrdersOnASymbol(const ApiGateway::TradingPairSymbol& symbol) const {
  LOG(INFO) << "cancelAllOrdersOnASymbol symbol=" << symbol.val_;

  const std::string queryString = "symbol=" + symbol.val_ + "&recvWindow=5000" + "&timestamp=" + getTimeSinceEpoch();
  const std::string signature = encryption.generateSignature(queryString);
  const std::string signedQuery = queryString + "&signature=" + signature;
  const std::string url = binanceTestnetBaseUrl + "/api/v3/openOrders" + "?" + signedQuery;

  const auto response = Http::Http().del(url, "X-MBX-APIKEY: " + encryption.getApiKey());

  if (response.statusCode != statusCodeOk) {
    LOG(WARNING) << "cancelAllOrdersOnASymbol returned NOK. Msg=" << response.body.val_;
    return ApiGateway::OrderResult::Failure;
  }
  return ApiGateway::OrderResult::Success;
}

}  // namespace MarketService::Binance
