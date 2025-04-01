#include <glog/logging.h>

#include <algorithm>
#include <nlohmann/json.hpp>
#include <ranges>

#include "Assets.hpp"
#include "Wallet.hpp"

namespace Wallet {

Assets Wallet::getAssets() const {
  const auto marketWallet = marketService_.getAssets();

  Assets domainWallet;
  const auto domainWalletView = std::views::transform(marketWallet, [](const auto& elem) {
    return SingleAsset{.assetSymbol = AssetSymbol{elem.assetSymbol.val_},
                       .freeAmount = FreeAssetAmount{elem.freeAmount.val_},
                       .usdtValue = ValueInUsdt{"1.1"}};
  });
  std::ranges::copy(domainWalletView, std::back_inserter(domainWallet));

  return domainWallet;

  // TODO 1: check if trading pair available using GET /api/v3/exchangeInfo - of so, fetch its price
  // TODO 2: merge assets with prices (separate component Market?) and send it to frontend
  // std::vector<std::string> symbols;
  // for (const auto& singleBalance : accountJson.at("balances")) {
  //   symbols.push_back(singleBalance.at("assetSymbol").get<std::string>() + "USDT");
  // }
  // const auto prices = marketService_.getPrices(symbols);
  // std::map<std::string, std::string> symbolToPriceMap;
  // for (const auto& singlePrice : prices) {
  //   symbolToPriceMap[singlePrice.symbol] = singlePrice.price;
  // }
  // double usdtValueDouble = std::stod(freeAmount) * std::stod(symbolToPriceMap.at(assetSymbol));
}

}  // namespace Wallet