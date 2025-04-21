#pragma once

#include "ApiGateway/Assets.hpp"
#include "AssetValues.hpp"

namespace Wallet {

class IWallet {
 public:
  virtual ApiGateway::Assets getOwnedAssets() const = 0;
  virtual AssetValues getOwnedAssetValues(const ApiGateway::AssetSymbol& quoteAsset) const = 0;
};

}  // namespace Wallet