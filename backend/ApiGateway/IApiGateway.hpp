#pragma once

#include "AssetSymbol.hpp"
#include "AssetSymbols.hpp"
#include "Assets.hpp"
#include "AvailableQuoteAssets.hpp"
#include "OrderResult.hpp"
#include "Orders.hpp"

namespace ApiGateway {

class IApiGateway {
 public:
  virtual Assets getAssets() const = 0;

  virtual OrderResult makeOrder(const AssetSymbol& selectedBaseAsset, const AssetSymbol& selectedQuoteAsset,
                                const OrderSide& orderSide, const AssetQuantity& baseAssetAmount) const = 0;

  virtual Orders getOpenOrders() const = 0;

  virtual AvailableQuoteAssets getAvailableQuoteAssets(const AssetSymbol& baseAsset) const = 0;

  virtual AssetSymbols getAvailableBaseAssets(const std::optional<AssetSymbol>& quoteAsset = std::nullopt) const = 0;
};

}  // namespace ApiGateway