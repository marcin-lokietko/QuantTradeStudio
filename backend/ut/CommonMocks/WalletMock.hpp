#pragma once

#include <gmock/gmock.h>

#include "Wallet/IWallet.hpp"

namespace Wallet {

class WalletMock : public Wallet::IWallet {
 public:
  MOCK_METHOD(ApiGateway::Assets, getOwnedAssets, (), (const, override));
  MOCK_METHOD(AssetValues, getOwnedAssetValues, (const ApiGateway::AssetSymbol& quoteAsset), (const, override));
};
}  // namespace Wallet