#include "Wallet.hpp"

namespace ApiGateway::Conversion {

namespace {
WalletItem toApi(const Account::WalletItem& walletItem) {
  return {Asset{walletItem.asset.val_}, FreeAssetAmount{walletItem.freeAmount.val_},
          ValueInUsdt{walletItem.usdtValue.val_}};
}
}  // namespace

Wallet toApi(const Account::Wallet& wallet) {
  Wallet retWallet;

  for (const auto& walletItem : wallet) {
    retWallet.push_back(toApi(walletItem));
  }

  return retWallet;
}

}  // namespace ApiGateway::Conversion