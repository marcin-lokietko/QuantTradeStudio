#include "AssetPrices.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, MarketService::AssetPrices& assetPrices,
              const TradingPairSymbolDecoder& tradingPairSymbolDecoder) {
  assetPrices.clear();
  assetPrices.reserve(j.size());

  for (const auto& singleAssetPrice : j) {
    assetPrices.push_back(
        {.assetPair = tradingPairSymbolDecoder.decode(singleAssetPrice.at("symbol").get<std::string>()),
         .price = ApiGateway::Price{singleAssetPrice.at("price").get<std::string>()}});
  }
}

}  // namespace MarketService::Binance::Conversion