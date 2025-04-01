#include "Wallet.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, MarketService::Wallet& wallet) {
  wallet.clear();
  for (const auto& item : j) {
    wallet.push_back({.asset = item.at("asset").get<std::string>(),
                      .freeAmount = item.at("free").get<std::string>(),
                      .marketId = MarketId::Binance});
  }
}

}  // namespace MarketService::Binance::Conversion