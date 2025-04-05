#pragma once

#include "Assets.hpp"

namespace ApiGateway {

enum class OrderResult
{
  Success = 0,
  Failure
};

class IApiGateway {
 public:
  virtual Assets getAssets() const = 0;

  virtual OrderResult makeOrder(const AssetSymbol& assetToBuy, const AssetSymbol& assetToSpend,
                                const AssetQuantity& quantityToBuy) const = 0;
};

}  // namespace ApiGateway