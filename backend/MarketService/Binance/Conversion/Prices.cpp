#include "Prices.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, MarketService::Prices& prices) {
  prices.clear();
  prices.reserve(j.size());

  for (const auto& singlePrice : j) {
    std::string assetPair = singlePrice.at("symbol").get<std::string>();
    std::string price = singlePrice.at("price").get<std::string>();
    prices.push_back({.assetPair = AssetPair{std::move(assetPair)}, .price = Price{std::move(price)}});
  }
}

}  // namespace MarketService::Binance::Conversion