#include <glog/logging.h>

#include "ApiGateway.hpp"
#include "ApiGateway/Conversion/Assets.hpp"
#include "Price.hpp"
#include "TradingPairSymbol.hpp"

namespace ApiGateway {

ApiGateway::ApiGateway(MarketService::IMarketService& marketService, Wallet::IWallet& wallet)
    : marketService_(marketService), wallet_(wallet){};

Assets ApiGateway::getAssets() const { return Conversion::toApi(wallet_.getAssets()); }

OrderResult ApiGateway::makeOrder(const AssetSymbol& assetToBuy, const AssetSymbol& assetToSpend,
                                  const AssetQuantity& quantityToBuy) const {
  TradingPairSymbol tradingPairSymbol{assetToBuy.val_ + assetToSpend.val_};

  const auto price = marketService_.getPrice(tradingPairSymbol);

  marketService_.makeOrder(TradingPairSymbol{assetToBuy.val_ + assetToSpend.val_}, quantityToBuy, price);

  return OrderResult::Success;
}

}  // namespace ApiGateway