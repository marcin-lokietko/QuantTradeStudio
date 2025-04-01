#include <glog/logging.h>

#include "ApiGateway.hpp"
#include "ApiGateway/Conversion/Assets.hpp"

namespace ApiGateway {

ApiGateway::ApiGateway(MarketService::IMarketService& marketService, Wallet::IWallet& wallet)
    : marketService_(marketService), wallet_(wallet){};

Assets ApiGateway::getAssets() const { return Conversion::toApi(wallet_.getAssets()); }

}  // namespace ApiGateway