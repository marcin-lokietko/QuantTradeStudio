#pragma once

#include "Account/IAccount.hpp"
#include "IApiGateway.hpp"
#include "MarketService/IMarketService.hpp"

namespace ApiGateway {

class ApiGateway : public IApiGateway {
 public:
  ApiGateway(MarketService::IMarketService& marketService, Account::IAccount& account);

  Wallet getWallet() const override;

 public:
  MarketService::IMarketService& marketService_;
  Account::IAccount& account_;
};

}  // namespace ApiGateway