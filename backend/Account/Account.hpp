#pragma once

#include "IAccount.hpp"
#include "MarketService/IMarketService.hpp"

namespace Account {

class Account : public IAccount {
 public:
  Account(const MarketService::IMarketService& marketService) : marketService_(marketService) {}

  Wallet getWallet() const override;

 private:
  const MarketService::IMarketService& marketService_;
};

}  // namespace Account