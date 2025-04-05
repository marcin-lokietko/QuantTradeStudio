#include "TradingPairs.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, MarketService::TradingPairs& tradingPairs) {
  tradingPairs.clear();
  tradingPairs.reserve(j.size());

  for (const auto& singlePrice : j.at("symbols")) {
    tradingPairs.push_back({.symbol = ApiGateway::TradingPairSymbol{singlePrice.at("symbol").get<std::string>()},
                            .baseAsset = AssetSymbol{singlePrice.at("baseAsset").get<std::string>()},
                            .quoteAsset = AssetSymbol{singlePrice.at("quoteAsset").get<std::string>()}});
  }
}

}  // namespace MarketService::Binance::Conversion