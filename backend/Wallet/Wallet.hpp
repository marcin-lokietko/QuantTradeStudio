#pragma once

#include "IWallet.hpp"
#include "MarketService/IMarketService.hpp"

namespace Wallet {

class Wallet : public IWallet {
 public:
  Wallet(const MarketService::IMarketService& marketService) : marketService_(marketService) {}

  Assets getAssets() const override;

 private:
  const MarketService::IMarketService& marketService_;
};

}  // namespace Wallet