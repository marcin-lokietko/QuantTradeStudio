#include "Prices.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, MarketService::Prices& prices) {
  prices.clear();
  prices.reserve(j.size());

  for (const auto& singlePrice : j) {
    prices.push_back({.assetPair = TradingPairSymbol{singlePrice.at("symbol").get<std::string>()},
                      .price = Price{singlePrice.at("price").get<std::string>()}});
  }
}

}  // namespace MarketService::Binance::Conversion