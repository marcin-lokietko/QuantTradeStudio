#pragma once

#include "ApiGateway/Assets.hpp"
#include "AssetValues.hpp"

namespace Wallet {

class IWallet {
 public:

  MOCK_METHOD(ApiGateway::Assets, getOwnedAssets, (), (const, override));
  MOCK_METHOD(AssetValues, getOwnedAssetValues, (const ApiGateway::AssetSymbol& quoteAsset), (const, override));
};

}  // namespace Wallet