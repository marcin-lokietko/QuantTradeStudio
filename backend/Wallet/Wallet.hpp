#pragma once

#include "AssetValues.hpp"
#include "IWallet.hpp"
#include "MarketService/IMarketService.hpp"

namespace Wallet {

class Wallet : public IWallet {
 public:
  Wallet(const MarketService::IMarketService& marketService) : marketService_(marketService) {}

  ApiGateway::Assets getOwnedAssets() const override;

  AssetValues getOwnedAssetValues(const ApiGateway::AssetSymbol& quoteAsset) const override;

 private:
  MarketService::AssetPricesMap getSymbolToPriceMap(const ApiGateway::AssetSymbol& quoteAsset) const;
  const MarketService::IMarketService& marketService_;
};

}  // namespace Wallet