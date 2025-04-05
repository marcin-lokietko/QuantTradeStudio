#pragma once

#include "IApiGateway.hpp"
#include "MarketService/IMarketService.hpp"
#include "Wallet/IWallet.hpp"

namespace ApiGateway {

class ApiGateway : public IApiGateway {
 public:
  ApiGateway(MarketService::IMarketService& marketService, Wallet::IWallet& wallet);

  Assets getAssets() const override;

  virtual OrderResult makeOrder(const AssetSymbol& assetToBuy, const AssetSymbol& assetToSpend,
                                const AssetQuantity& quantityToBuy) const;

 public:
  MarketService::IMarketService& marketService_;
  Wallet::IWallet& wallet_;
};

}  // namespace ApiGateway