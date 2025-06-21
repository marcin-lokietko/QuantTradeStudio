#include "Wallet.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <nlohmann/json.hpp>
#include <ranges>
#include <string>

#include "ApiGateway/Assets.hpp"
#include "AssetValues.hpp"

namespace Wallet {

ApiGateway::Assets Wallet::getOwnedAssets() const {
  const auto symbolToPriceMap = getSymbolToPriceMap(ApiGateway::AssetSymbol{"USDT"});
  const auto marketOwnedAssets = marketService_.getOwnedAssets();

  ApiGateway::Assets userAssets;
  for (const auto& singleMarketAsset : marketOwnedAssets) {
    ApiGateway::SingleAsset singleAsset{.assetSymbol = ApiGateway::AssetSymbol{singleMarketAsset.assetSymbol.val_},
                                        .freeQuantity = ApiGateway::AssetQuantity{singleMarketAsset.freeQuantity.val_},
                                        .usdtValue = ApiGateway::Value{""}};
    if (const auto it = symbolToPriceMap.find(ApiGateway::TradingPairSymbol{singleAsset.assetSymbol.val_ + "USDT"});
        it != symbolToPriceMap.end()) {
      double usdtValueDouble = std::stod(singleAsset.freeQuantity.val_) * std::stod(it->second.val_);
      singleAsset.usdtValue = ApiGateway::Value{std::to_string(usdtValueDouble)};
    }
    userAssets.push_back(singleAsset);
  }
  return userAssets;
}

AssetValues Wallet::getOwnedAssetValues(const ApiGateway::AssetSymbol& quoteAsset) const {
  const auto symbolToPriceMap = getSymbolToPriceMap(quoteAsset);
  const auto marketOwnedAssets = marketService_.getOwnedAssets();

  AssetValues assetValues;
  for (const auto& singleMarketAsset : marketOwnedAssets) {
    SingleAssetValue singleAssetValue{.baseSymbol = singleMarketAsset.assetSymbol, .quoteAsset = quoteAsset};
    if (const auto it =
            symbolToPriceMap.find(ApiGateway::TradingPairSymbol{singleMarketAsset.assetSymbol.val_ + "USDT"});
        it != symbolToPriceMap.end()) {
      double usdtValueDouble = std::stod(singleMarketAsset.freeQuantity.val_) * std::stod(it->second.val_);
      singleAssetValue.value = ApiGateway::Value{std::to_string(usdtValueDouble)};
    }
    assetValues.push_back(singleAssetValue);
  }
  return assetValues;
}

MarketService::AssetPricesMap Wallet::getSymbolToPriceMap(const ApiGateway::AssetSymbol& quoteAsset) const {
  std::vector<ApiGateway::TradingPairSymbol> supportedTradingPairs;
  for (const auto& pair : marketService_.getTradingPairsWithQuoteAsset(quoteAsset)) {
    supportedTradingPairs.push_back(pair.symbol);
  }
  return asMap(marketService_.getPrices(supportedTradingPairs));
}

}  // namespace Wallet