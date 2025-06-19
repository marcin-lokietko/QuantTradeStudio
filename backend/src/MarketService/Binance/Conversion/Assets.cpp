#include "Assets.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, MarketService::Assets& assets) {
  assets.clear();
  for (const auto& item : j) {
    assets.push_back(
        MarketService::SingleAsset{.assetSymbol = ApiGateway::AssetSymbol{item.at("asset").get<std::string>()},
                                   .freeQuantity = AssetQuantity{item.at("free").get<std::string>()},
                                   .marketId = MarketId::Binance});
  }
}

}  // namespace MarketService::Binance::Conversion