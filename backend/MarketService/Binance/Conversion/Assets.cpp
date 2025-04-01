#include "Assets.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, MarketService::Assets& assets) {
  assets.clear();
  for (const auto& item : j) {
    assets.push_back({.assetSymbol = item.at("asset").get<std::string>(),
                      .freeAmount = item.at("free").get<std::string>(),
                      .marketId = MarketId::Binance});
  }
}

}  // namespace MarketService::Binance::Conversion