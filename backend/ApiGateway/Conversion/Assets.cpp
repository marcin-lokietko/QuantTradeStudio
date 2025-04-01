#include "Assets.hpp"

namespace ApiGateway::Conversion {

namespace {
SingleAsset toApi(const Wallet::SingleAsset& singleAsset) {
  return {AssetSymbol{singleAsset.assetSymbol.val_}, FreeAssetAmount{singleAsset.freeAmount.val_},
          ValueInUsdt{singleAsset.usdtValue.val_}};
}
}  // namespace

Assets toApi(const Wallet::Assets& assets) {
  Assets retWallet;

  for (const auto& singleAsset : assets) {
    retWallet.push_back(toApi(singleAsset));
  }

  return retWallet;
}

}  // namespace ApiGateway::Conversion