#include "Wallet.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, MarketService::Wallet& wallet) {
  wallet.clear();
  for (const auto& item : j) {
    std::string asset = item.at("asset").get<std::string>();
    std::string amountFree = item.at("free").get<std::string>();
    wallet.push_back({.asset = asset, .amountFree = amountFree, .marketId = MarketId::Binance});
  }
}

}  // namespace MarketService::Binance::Conversion