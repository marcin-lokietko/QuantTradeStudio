#include <glog/logging.h>

#include <nlohmann/json.hpp>

#include "Account.hpp"
#include "Wallet.hpp"

namespace Account {

Wallet Account::getWallet() const {
  const std::string accountResponse = stockMarketService_.getAccountData();
  const auto accountJson = nlohmann::json::parse(accountResponse);

  std::vector<std::string> symbols;
  for (const auto& singleBalance : accountJson.at("balances")) {
    symbols.push_back(singleBalance.at("asset").get<std::string>() + "USDT");
  }

  // TODO - check if trading pair available using GET /api/v3/exchangeInfo - of so, fetch its price
  // const auto prices = stockMarketService_.getPrices(symbols);
  // std::map<std::string, std::string> symbolToPriceMap;
  // for (const auto& singlePrice : prices) {
  //   symbolToPriceMap[singlePrice.symbol] = singlePrice.price;
  // }

  Wallet wallet;
  for (const auto& singleBalance : accountJson.at("balances")) {
    std::string asset = singleBalance.at("asset").get<std::string>();
    std::string amountFree = singleBalance.at("free").get<std::string>();
    // double usdtValueDouble = std::stod(amountFree) * std::stod(symbolToPriceMap.at(asset));
    double usdtValueDouble = 1.23;
    wallet.push_back({.asset = asset, .amountFree = amountFree, .usdtValue = std::to_string(usdtValueDouble)});
  }
  return wallet;
}

}  // namespace Account