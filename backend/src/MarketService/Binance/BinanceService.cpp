#include <spdlog/spdlog.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <ranges>

#include "BinanceService.hpp"
#include "MarketService/Binance/Conversion/AssetPrices.hpp"
#include "MarketService/Binance/Conversion/AssetQuantities.hpp"
#include "MarketService/Binance/Conversion/KlineSequence.hpp"
#include "MarketService/Binance/Conversion/Orders.hpp"
#include "MarketService/Binance/Conversion/TradingPairs.hpp"
#include "MarketService/TradingPairSymbolDecoder.hpp"
#include "Utils/ToString.hpp"

namespace MarketService::Binance {

namespace {
constexpr Http::HttpStatusCode statusCodeOk{200};
constexpr Http::HttpStatusCode statusCodeRequestRateLimitBroken{429};
}  // namespace

Time BinanceService::getServerTime() {
  SPDLOG_INFO("getServerTime called");

  const std::string timeUrl = binanceUrlPrefix_.val_ + "/time";

  return Time{http_.get(timeUrl, "")};
}

ApiGateway::Price BinanceService::getPrice(const ApiGateway::TradingPairSymbol& tradingPairSymbol) {
  SPDLOG_INFO("getPrice called: tradingPairSymbol={}", toString(tradingPairSymbol));

  std::string url = binanceUrlPrefix_.val_ + "/ticker/price?symbol=" + toString(tradingPairSymbol);
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
    symbolsString += toString(singleSymbol);
    symbolsString += '"';
  }
  symbolsString += "]";
  const std::string url = binanceUrlPrefix_.val_ + "/ticker/price?symbols=" + symbolsString;
  const auto response = http_.get(url, "");

  AssetPrices assetPrices;
  try {
    Conversion::fromJson(nlohmann::json::parse(response), assetPrices, TradingPairSymbolDecoder(getAllTradingPairs()));
  } catch (const nlohmann::json::exception& e) {
    SPDLOG_CRITICAL("getPrices FAILED; could not deserialize Binance response; error: {}", e.what());
    throw;
  }

  SPDLOG_INFO("getPrices result: assetPrices={}", ::toString(assetPrices));
  return assetPrices;
}

MarketService::KlineSequence BinanceService::getKlines(const ApiGateway::TradingPairSymbol& symbol,
                                                       const KlineInterval interval,
                                                       const std::chrono::system_clock::time_point startTime,
                                                       const std::chrono::system_clock::time_point endTime) const {
  SPDLOG_INFO("getKlines called: symbol={} interval={} startTime={} endTime={}", toString(symbol), toString(interval),
              startTime, endTime);

  const auto startTimeMilliseconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(startTime.time_since_epoch()).count();
  const auto endTimeMilliseconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();

  // The Binance /api/v3/klines endpoint sometimes returns klines starting after the specified startTime or ending
  // before endTime (i.e. it's exclusive). For it to be inclusive, it's needed to subtract 1 interval from the
  // startTime and add 1 interval to the endTime.
  const auto startTimeMillisecondsAdjusted = startTimeMilliseconds - toMilliseconds(interval).count();
  const auto endTimeMillisecondsAdjusted = endTimeMilliseconds + toMilliseconds(interval).count();

  const std::string klinesUrl = binanceUrlPrefix_.val_ + "/klines?symbol=" + toString(symbol) +
                                "&interval=" + toString(interval) +
                                "&startTime=" + std::to_string(startTimeMillisecondsAdjusted) +
                                "&endTime=" + std::to_string(endTimeMillisecondsAdjusted) + "&limit=1000";
  const auto response = http_.get(klinesUrl, "");

  MarketService::KlineSequence sequence;
  try {
    Conversion::fromJson(nlohmann::json::parse(response), sequence);
  } catch (const nlohmann::json::exception& e) {
    SPDLOG_CRITICAL("getKlines FAILED; could not deserialize Binance response; error: {}", e.what());
    throw;
  }

  SPDLOG_INFO("getKlines result: sequence={}", ::toString(sequence));
  return sequence;
}

ApiGateway::AssetQuantities BinanceService::getOwnedAssetsQuantity() const {
  SPDLOG_INFO("getOwnedAssetsQuantity called");

  const auto accountUrl = getAccountUrl();
  const auto accountString = http_.get(accountUrl, "X-MBX-APIKEY: " + encryption_.getApiKey());
  const auto accountJson = nlohmann::json::parse(accountString);

  ApiGateway::AssetQuantities assets;
  try {
    MarketService::Binance::Conversion::fromJson(accountJson.at("balances"), assets);
  } catch (const nlohmann::json::exception& e) {
    SPDLOG_CRITICAL("getOwnedAssetsQuantity FAILED; could not deserialize Binance response; error: {}", e.what());
    throw;
  }
  SPDLOG_INFO("getOwnedAssetsQuantity result: assets={}", ::toString(assets));
  return assets;
}

ApiGateway::AssetValues BinanceService::getOwnedAssetsQuantityAndValue() const {
  const auto symbolToPriceMap = getSymbolToPriceMap(ApiGateway::AssetSymbol{"USDT"});
  const auto marketOwnedAssets = getOwnedAssetsQuantity();

  ApiGateway::AssetValues userAssets;
  for (const auto& singleMarketAsset : marketOwnedAssets) {
    ApiGateway::SingleAssetValue singleAsset{
        .assetSymbol = ApiGateway::AssetSymbol{singleMarketAsset.assetSymbol.val_},
        .freeQuantity = ApiGateway::AssetQuantity{singleMarketAsset.freeQuantity.val_},
        .usdtValue = ApiGateway::Value{""}};
    if (const auto it = symbolToPriceMap.find(
            ApiGateway::TradingPairSymbol{singleAsset.assetSymbol, ApiGateway::AssetSymbol{"USDT"}});
        it != symbolToPriceMap.end()) {
      double usdtValueDouble = std::stod(singleAsset.freeQuantity.val_) * std::stod(it->second.val_);
      singleAsset.usdtValue = ApiGateway::Value{std::to_string(usdtValueDouble)};
    }
    userAssets.push_back(singleAsset);
  }
  return userAssets;
}

AssetValues BinanceService::getOwnedAssetValues(const ApiGateway::AssetSymbol& quoteAsset) const {
  const auto symbolToPriceMap = getSymbolToPriceMap(quoteAsset);
  const auto marketOwnedAssets = getOwnedAssetsQuantity();

  AssetValues assetValues;
  for (const auto& singleMarketAsset : marketOwnedAssets) {
    SingleAssetValue singleAssetValue{.baseSymbol = singleMarketAsset.assetSymbol, .quoteAsset = quoteAsset};
    if (const auto it = symbolToPriceMap.find(
            ApiGateway::TradingPairSymbol{singleMarketAsset.assetSymbol, ApiGateway::AssetSymbol{"USDT"}});
        it != symbolToPriceMap.end()) {
      double usdtValueDouble = std::stod(singleMarketAsset.freeQuantity.val_) * std::stod(it->second.val_);
      singleAssetValue.value = ApiGateway::Value{std::to_string(usdtValueDouble)};
    }
    assetValues.push_back(singleAssetValue);
  }
  return assetValues;
}

ApiGateway::OrderResult BinanceService::makeOrder(const ApiGateway::TradingPairSymbol& symbol,
                                                  const ApiGateway::OrderSide& orderSide,
                                                  const ApiGateway::AssetQuantity& quantity,
                                                  const ApiGateway::Price& price) {
  SPDLOG_INFO("makeOrder called: symbol={} orderSide={} quantity={} price={}", toString(symbol), toString(orderSide),
              quantity, price);

  std::string orderSideString = toString(orderSide);
  std::ranges::transform(orderSideString, orderSideString.begin(), [](unsigned char c) { return std::toupper(c); });

  const std::string queryString = "symbol=" + toString(symbol) + "&side=" + orderSideString +
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
  SPDLOG_INFO("makeMarketTypeOrderWithQuoteQuantity called: symbol={} orderSide={} quoteQuantity={}", toString(symbol),
              toString(orderSide), quoteQuantity);

  std::string orderSideString = toString(orderSide);
  std::ranges::transform(orderSideString, orderSideString.begin(), [](unsigned char c) { return std::toupper(c); });

  const std::string queryString = "symbol=" + toString(symbol) + "&side=" + orderSideString +
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
  try {
    Conversion::fromJson(nlohmann::json::parse(tradingPairsString), tradingPairs);
  } catch (const nlohmann::json::exception& e) {
    SPDLOG_CRITICAL("getAllTradingPairs FAILED; could not deserialize Binance response; error: {}", e.what());
    throw;
  }
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
    Conversion::fromJson(nlohmann::json::parse(openOrdersString), orders,
                         TradingPairSymbolDecoder(getAllTradingPairs()));
  } catch (const nlohmann::json::exception& e) {
    SPDLOG_CRITICAL("getOpenOrders FAILED; could not deserialize Binance response; error: {}", e.what());
    throw;
  }
  SPDLOG_INFO("getOpenOrders result: orders={}", ::toString(orders));
  return orders;
}

ApiGateway::OrderResult BinanceService::cancelAllOrdersOnASymbol(const ApiGateway::TradingPairSymbol& symbol) const {
  SPDLOG_INFO("cancelAllOrdersOnASymbol called: symbol={}", toString(symbol));

  const std::string queryString =
      "symbol=" + toString(symbol) + "&recvWindow=5000" + "&timestamp=" + std::to_string(time_.getTimeSinceEpoch());
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

MarketService::AssetPricesMap BinanceService::getSymbolToPriceMap(const ApiGateway::AssetSymbol& quoteAsset) const {
  std::vector<ApiGateway::TradingPairSymbol> supportedTradingPairs = getTradingPairsWithQuoteAsset(quoteAsset);
  return asMap(getPrices(supportedTradingPairs));
}
}  // namespace MarketService::Binance
