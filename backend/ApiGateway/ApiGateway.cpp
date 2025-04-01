#include <glog/logging.h>

#include "ApiGateway.hpp"
#include "ApiGateway/Conversion/Wallet.hpp"

namespace ApiGateway {

ApiGateway::ApiGateway(MarketService::IMarketService& marketService, Account::IAccount& account)
    : marketService_(marketService), account_(account){};

Wallet ApiGateway::getWallet() const { return Conversion::toApi(account_.getWallet()); }

}  // namespace ApiGateway