#pragma once

#include "IAccount.hpp"
#include "StockMarketService/IStockMarketService.hpp"

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