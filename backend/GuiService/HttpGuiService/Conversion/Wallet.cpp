#include "Wallet.hpp"

namespace GuiService::HttpGuiService::Conversion {

namespace {
nlohmann::json toJson(const ApiGateway::WalletItem& walletItem) {
  nlohmann::json j;
  j = nlohmann::json{{"asset", walletItem.asset.val_},
                     {"freeAmount", walletItem.freeAmount.val_},
                     {"usdtValue", walletItem.usdtValue.val_}};
  return j;
}
}  // namespace

nlohmann::json toJson(const ApiGateway::Wallet& wallet) {
  nlohmann::json j = nlohmann::json::array();
  for (const auto& walletItem : wallet) {
    j.push_back(toJson(walletItem));
  }
  return j;
}

}  // namespace GuiService::HttpGuiService::Conversion