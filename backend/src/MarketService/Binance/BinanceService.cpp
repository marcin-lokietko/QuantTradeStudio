#include "BinanceService.hpp"

#include <spdlog/spdlog.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <ranges>

#include "MarketService/Binance/Conversion/AssetPrices.hpp"
#include "MarketService/Binance/Conversion/Assets.hpp"
#include "MarketService/Binance/Conversion/Orders.hpp"
#include "MarketService/Binance/Conversion/TradingPairs.hpp"
#include "Utils/ToString.hpp"

namespace MarketService::Binance {

namespace {
constexpr Http::HttpStatusCode statusCodeOk{200};
constexpr Http::HttpStatusCode statusCodeRequestRateLimitBroken{429};
}  // namespace

std::string BinanceService::getServerTime() {
  SPDLOG_INFO("getServerTime called");

  const std::string timeUrl = binanceUrlPrefix_.val_ + "/time";

  return http_.get(timeUrl, "");
}

ApiGateway::Price BinanceService::getPrice(const ApiGateway::TradingPairSymbol& tradingPairSymbol) {
  SPDLOG_INFO("getPrice called: tradingPairSymbol={}", tradingPairSymbol);

  std::string url = binanceUrlPrefix_.val_ + "/ticker/price?symbol=" + tradingPairSymbol.val_;
  const auto response = http_.get(url, "");

  const auto jsonResponse = nlohmann::json::parse(response);
  ApiGateway::Price result{""};
  if (jsonResponse.contains("price")) {
    result = ApiGateway::Price{jsonResponse["price"]};
  } else {
    SPDLOG_ERROR("Failed to retrieve price");
  }
  SPDLOG_INFO("getPrice result: result={}", result);

  return result;
}

AssetPrices BinanceService::getPrices(const std::vector<ApiGateway::TradingPairSymbol>& tradingPairSymbols) const {
  SPDLOG_INFO("getPrices called: tradingPairSymbols={}", ::toString(tradingPairSymbols));

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
  const std::string url = binanceUrlPrefix_.val_ + "/ticker/price?symbols=" + symbolsString;
  const auto response = http_.get(url, "");

  AssetPrices assetPrices;
  Conversion::fromJson(nlohmann::json::parse(response), assetPrices);
  SPDLOG_INFO("getPrices result: assetPrices={}", ::toString(assetPrices));
  return assetPrices;
}

// e.g. symbol="BTCUSDT", interval="1h"
MarketService::KlineSequence BinanceService::getKlines(const std::string& symbol, const std::string& interval) {
  SPDLOG_INFO("getKlines called: symbol={} interval={}", symbol, interval);

  const std::string klinesUrl =
      binanceUrlPrefix_.val_ + "/klines?symbol=" + symbol + "&interval=" + interval + "&limit=1000";
  const auto klinesString = http_.get(klinesUrl, "");
  const auto klinesJson = nlohmann::json::parse(klinesString);

  MarketService::KlineSequence sequence;
  sequence.reserve(klinesJson.size());

  for (const auto& kline : klinesJson) {
    std::string closePrice = kline[4].get<std::string>();
    uint64_t closeTime = kline[6].get<uint64_t>();
    sequence.push_back({.closeTime = closeTime, .closePrice = closePrice});
  }

  SPDLOG_INFO("getKlines result: sequence={}", ::toString(sequence));
  return sequence;
}

Assets BinanceService::getOwnedAssets() const {
  SPDLOG_INFO("getOwnedAssets called");

  const auto accountUrl = getAccountUrl();
  const auto accountString = http_.get(accountUrl, "X-MBX-APIKEY: " + encryption_.getApiKey());
  const auto accountJson = nlohmann::json::parse(accountString);

  Assets assets;
  MarketService::Binance::Conversion::fromJson(accountJson.at("balances"), assets);

  SPDLOG_INFO("getOwnedAssets result: assets={}", ::toString(assets));
  return assets;
}

ApiGateway::OrderResult BinanceService::makeOrder(const ApiGateway::TradingPairSymbol& symbol,
                                                  const ApiGateway::OrderSide& orderSide,
                                                  const ApiGateway::AssetQuantity& quantity,
                                                  const ApiGateway::Price& price) {
  SPDLOG_INFO("makeOrder called: symbol={} orderSide={} quantity={} price={}", symbol, toString(orderSide), quantity,
              price);

  std::string orderSideString = toString(orderSide);
  std::ranges::transform(orderSideString, orderSideString.begin(), [](unsigned char c) { return std::toupper(c); });

  const std::string queryString = "symbol=" + symbol.val_ + "&side=" + orderSideString +
                                  "&type=LIMIT&timeInForce=GTC&quantity=" + quantity.val_ + "&price=" + price.val_ +
                                  "&recvWindow=5000&timestamp=" + std::to_string(time_.getTimeSinceEpoch());

  const auto orderUrl = getOrderUrl(queryString);
  const auto response = http_.post(orderUrl, "X-MBX-APIKEY: " + encryption_.getApiKey());

  ApiGateway::OrderResult result{ApiGateway::OrderResult::Failure};
  if (response.statusCode == statusCodeOk) {
    result = ApiGateway::OrderResult::Success;
  } else {
    SPDLOG_WARN("makeOrder returned NOK. Msg={}", response.body.val_);
  }
  SPDLOG_INFO("getOwnedAssets result: result={}", toString(result));
  return result;
}

ApiGateway::OrderResult BinanceService::makeMarketTypeOrderWithQuoteQuantity(
    const ApiGateway::TradingPairSymbol& symbol, const ApiGateway::OrderSide& orderSide,
    const ApiGateway::AssetQuantity& quoteQuantity) const {
  SPDLOG_INFO("makeMarketTypeOrderWithQuoteQuantity called: symbol={} orderSide={} quoteQuantity={}", symbol,
              toString(orderSide), quoteQuantity);

  std::string orderSideString = toString(orderSide);
  std::ranges::transform(orderSideString, orderSideString.begin(), [](unsigned char c) { return std::toupper(c); });

  const std::string queryString = "symbol=" + symbol.val_ + "&side=" + orderSideString +
                                  "&type=MARKET&quoteOrderQty=" + quoteQuantity.val_ +
                                  "&recvWindow=5000&timestamp=" + std::to_string(time_.getTimeSinceEpoch());
  const auto orderUrl = getOrderUrl(queryString);
  const auto response = http_.post(orderUrl, "X-MBX-APIKEY: " + encryption_.getApiKey());

  ApiGateway::OrderResult result{ApiGateway::OrderResult::Failure};
  if (response.statusCode == statusCodeOk) {
    result = ApiGateway::OrderResult::Success;
  } else {
    SPDLOG_WARN("makeMarketTypeOrderWithQuoteQuantity returned NOK. Msg={}", response.body.val_);
  }
  SPDLOG_INFO("makeMarketTypeOrderWithQuoteQuantity result: result={}", toString(result));
  return result;
}

TradingPairs BinanceService::getAllTradingPairs() const {
  SPDLOG_INFO("getAllTradingPairs called");

  const std::string url = binanceUrlPrefix_.val_ + "/exchangeInfo";
  const auto tradingPairsString = http_.get(url, "");

  TradingPairs tradingPairs;
  Conversion::fromJson(nlohmann::json::parse(tradingPairsString), tradingPairs);

  SPDLOG_INFO("getAllTradingPairs result: tradingPairs={}", ::toString(tradingPairs));
  return tradingPairs;
}

TradingPairs BinanceService::getTradingPairsWithQuoteAsset(const ApiGateway::AssetSymbol& quoteAsset) const {
  SPDLOG_INFO("getTradingPairsWithQuoteAsset called: quoteAsset={}", quoteAsset);

  const auto tradingPairs = getAllTradingPairs();
  auto filteredPairsView =
      tradingPairs | std::views::filter([&quoteAsset](const auto& pair) { return pair.quoteAsset == quoteAsset; });

  const TradingPairs filteredTradingPairs{filteredPairsView.begin(), filteredPairsView.end()};
  SPDLOG_INFO("getTradingPairsWithQuoteAsset result: tradingPairs={}", ::toString(filteredTradingPairs));
  return filteredTradingPairs;
}

TradingPairs BinanceService::getTradingPairsWithBaseAsset(const ApiGateway::AssetSymbol& baseAsset) const {
  SPDLOG_INFO("getTradingPairsWithBaseAsset called: baseAsset={}", baseAsset);

  const auto tradingPairs = getAllTradingPairs();
  auto filteredPairsView =
      tradingPairs | std::views::filter([&baseAsset](const auto& pair) { return pair.baseAsset == baseAsset; });

  const TradingPairs filteredTradingPairs{filteredPairsView.begin(), filteredPairsView.end()};
  SPDLOG_INFO("getTradingPairsWithBaseAsset result: tradingPairs={}", ::toString(filteredTradingPairs));
  return filteredTradingPairs;
}

std::string BinanceService::getAccountUrl() const {
  const std::string timestamp = "timestamp=" + std::to_string(time_.getTimeSinceEpoch());
  const std::string signature = encryption_.generateSignature(timestamp);
  const std::string signedQuery = timestamp + "&signature=" + signature;
  return binanceUrlPrefix_.val_ + "/account?" + signedQuery;
}

std::string BinanceService::getOrderUrl(const std::string& queryString) const {
  const std::string signature = encryption_.generateSignature(queryString);
  const std::string signedQuery = queryString + "&signature=" + signature;
  return binanceUrlPrefix_.val_ + "/order?" + signedQuery;
}

ApiGateway::Orders BinanceService::getOpenOrders() const {
  SPDLOG_INFO("getOpenOrders called");

  const std::string timestamp = "timestamp=" + std::to_string(time_.getTimeSinceEpoch());
  const std::string signature = encryption_.generateSignature(timestamp);
  const std::string signedQuery = timestamp + "&signature=" + signature;
  const std::string url = binanceUrlPrefix_.val_ + "/openOrders" + "?" + signedQuery;

  const auto openOrdersString = http_.get(url, "X-MBX-APIKEY: " + encryption_.getApiKey());

  ApiGateway::Orders orders;
  try {
    Conversion::fromJson(nlohmann::json::parse(openOrdersString), orders);
  } catch (const std::exception& exc) {
    SPDLOG_ERROR("Error parsing response. Url={} Exception={}", url, exc.what());
  }
  SPDLOG_INFO("getOpenOrders result: orders={}", ::toString(orders));
  return orders;
}

ApiGateway::OrderResult BinanceService::cancelAllOrdersOnASymbol(const ApiGateway::TradingPairSymbol& symbol) const {
  SPDLOG_INFO("cancelAllOrdersOnASymbol called: symbol={}", symbol);

  const std::string queryString =
      "symbol=" + symbol.val_ + "&recvWindow=5000" + "&timestamp=" + std::to_string(time_.getTimeSinceEpoch());
  const std::string signature = encryption_.generateSignature(queryString);
  const std::string signedQuery = queryString + "&signature=" + signature;
  const std::string url = binanceUrlPrefix_.val_ + "/openOrders" + "?" + signedQuery;

  const auto response = http_.del(url, "X-MBX-APIKEY: " + encryption_.getApiKey());

  ApiGateway::OrderResult result{ApiGateway::OrderResult::Failure};
  if (response.statusCode == statusCodeOk) {
    result = ApiGateway::OrderResult::Success;
    SPDLOG_INFO("cancelAllOrdersOnASymbol result: success");
  } else {
    SPDLOG_ERROR("cancelAllOrdersOnASymbol returned NOK. Msg={}", response.body.val_);
  }
  SPDLOG_INFO("cancelAllOrdersOnASymbol result: result={}", toString(result));
  return result;
}

}  // namespace MarketService::Binance
