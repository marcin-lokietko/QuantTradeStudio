#include <glog/logging.h>

#include <algorithm>
#include <nlohmann/json.hpp>
#include <ranges>
#include <string>

#include "ApiGateway/Assets.hpp"
#include "Wallet.hpp"

namespace Wallet {

ApiGateway::Assets Wallet::getAssets() const {
  const auto marketAssets = marketService_.getAssets();

  std::vector<ApiGateway::TradingPairSymbol> supportedUsdtBasedTradingPairs;
  for (const auto& pair : marketService_.getTradingPairs(ApiGateway::AssetSymbol("USDT"))) {
    supportedUsdtBasedTradingPairs.push_back(pair.symbol);
  }

  const auto prices = marketService_.getPrices(supportedUsdtBasedTradingPairs);
  std::map<ApiGateway::TradingPairSymbol, MarketService::Price> symbolToPriceMap;
  for (const auto& singlePrice : prices) {
    symbolToPriceMap[singlePrice.assetPair] = singlePrice.price;
  }

  ApiGateway::Assets userAssets;
  for (const auto& singleMarketAsset : marketAssets) {
    ApiGateway::SingleAsset singleAsset{.assetSymbol = ApiGateway::AssetSymbol{singleMarketAsset.assetSymbol.val_},
                                        .freeQuantity = ApiGateway::AssetQuantity{singleMarketAsset.freeQuantity.val_},
                                        .usdtValue = ApiGateway::ValueInUsdt{""}};
    if (const auto it = symbolToPriceMap.find(ApiGateway::TradingPairSymbol{singleAsset.assetSymbol.val_ + "USDT"});
        it != symbolToPriceMap.end()) {
      double usdtValueDouble = std::stod(singleAsset.freeQuantity.val_) * std::stod(it->second.val_);
      singleAsset.usdtValue = ApiGateway::ValueInUsdt{std::to_string(usdtValueDouble)};
    }
    userAssets.push_back(singleAsset);
  }
  return userAssets;
}

}  // namespace Wallet