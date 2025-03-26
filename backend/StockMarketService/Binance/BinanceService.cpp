#include <glog/logging.h>

#include <fstream>
#include <nlohmann/json.hpp>

#include "BinanceService.h"
#include "Http/Http.h"

namespace StockMarketService::Binance {

namespace {
std::string getTimeSinceEpoch() { return std::to_string(time(nullptr) * 1000); }
}  // namespace

std::string BinanceService::getServerTime() {
  const std::string timeUrl = binanceTestnetBaseUrl + "/api/v3/time";
  return Http::Http().get(timeUrl, "");
}

std::string BinanceService::getPrice(const std::string& tradingPairSymbol) {
  std::string url = binanceTestnetBaseUrl + "/api/v3/ticker/price?symbol=" + tradingPairSymbol;
  const auto response = Http::Http().get(url, "");

  const auto jsonResponse = nlohmann::json::parse(response);
  if (jsonResponse.contains("price")) {
    return jsonResponse["price"];
  } else {
    LOG(ERROR) << "Failed to retrieve price.";
  }
  return "";
}

Prices BinanceService::getPrices(const std::vector<std::string>& tradingPairSymbols) const {
  std::string symbolsString = "[";
  for (const auto& singleSymbol : tradingPairSymbols) {
    if (symbolsString.back() != '[') {
      symbolsString += ",";
    }
    symbolsString += '"';
    symbolsString += singleSymbol;
    symbolsString += '"';
  }
  symbolsString += "]";
  const std::string url = binanceTestnetBaseUrl + "/api/v3/ticker/price?symbols=" + symbolsString;
  const auto response = Http::Http().get(url, "");

  const auto jsonResponse = nlohmann::json::parse(response);

  Prices prices;
  prices.reserve(jsonResponse.size());

  for (const auto& singlePrice : jsonResponse) {
    std::string symbol = singlePrice.at("symbol").get<std::string>();
    std::string price = singlePrice.at("price").get<std::string>();
    prices.push_back({.symbol = symbol, .price = price});
  }
  return prices;
}

// e.g. symbol="BTCUSDT", interval="1h"
StockMarketService::KlineSequence BinanceService::getKlines(const std::string& symbol, const std::string& interval) {
  const std::string klinesUrl =
      binanceTestnetBaseUrl + "/api/v3/klines?symbol=" + symbol + "&interval=" + interval + "&limit=1000";
  const auto klinesString = Http::Http().get(klinesUrl, "");
  const auto klinesJson = nlohmann::json::parse(klinesString);

  StockMarketService::KlineSequence sequence;
  sequence.reserve(klinesJson.size());

  for (const auto& kline : klinesJson) {
    std::string closePrice = kline[4].get<std::string>();
    uint64_t closeTime = kline[6].get<uint64_t>();
    sequence.push_back({.closeTime = closeTime, .closePrice = closePrice});
  }

  return sequence;
}

std::string BinanceService::getAccountData() const {
  const auto accountUrl = getAccountUrl();
  return Http::Http().get(accountUrl, "X-MBX-APIKEY: " + encryption.getApiKey());
}

// e.g. symbol="BTCUSDT", quantity="0.0001", price="100000.00"
void BinanceService::makeOrder(const std::string& symbol, const std::string& quantity, const std::string& price) {
  const std::string queryString = "symbol=" + symbol + "&side=BUY&type=LIMIT&timeInForce=GTC&quantity=" + quantity +
                                  "&price=" + price + "&recvWindow=5000&timestamp=" + getTimeSinceEpoch();
  const auto orderUrl = getOrderUrl(queryString);
  const auto response = Http::Http().post(orderUrl, "X-MBX-APIKEY: " + encryption.getApiKey());

  LOG(INFO) << "Order complete, response: " << response;
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

}  // namespace StockMarketService::Binance
