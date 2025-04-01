#pragma once

#include "IApiGateway.hpp"
#include "MarketService/IMarketService.hpp"
#include "Wallet/IWallet.hpp"

namespace ApiGateway {

class ApiGateway : public IApiGateway {
 public:
  ApiGateway(MarketService::IMarketService& marketService, Wallet::IWallet& wallet);

  Assets getAssets() const override;

 public:
  MarketService::IMarketService& marketService_;
  Wallet::IWallet& wallet_;
};

}  // namespace ApiGateway