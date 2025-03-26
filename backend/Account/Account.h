#pragma once

#include "IAccount.h"
#include "StockMarketService/IStockMarketService.h"

namespace Account {

class Account : public IAccount {
 public:
  Account(const StockMarketService::IStockMarketService& stockMarketService)
      : stockMarketService_(stockMarketService) {}

  Wallet getWallet() const override;

 private:
  const StockMarketService::IStockMarketService& stockMarketService_;
};

}  // namespace Account