#include "AssetQuantities.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, ApiGateway::AssetQuantities& assets) {
  assets.clear();
  for (const auto& item : j) {
    assets.push_back(
        ApiGateway::SingleAssetQuantity{.assetSymbol = ApiGateway::AssetSymbol{item.at("asset").get<std::string>()},
                                        .freeQuantity = ApiGateway::AssetQuantity{item.at("free").get<std::string>()},
                                        .marketId = ApiGateway::MarketId::Binance});
  }
}

}  // namespace MarketService::Binance::Conversion