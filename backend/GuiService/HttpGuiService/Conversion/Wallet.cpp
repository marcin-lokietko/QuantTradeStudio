#include "Wallet.hpp"

namespace GuiService::HttpGuiService::Conversion {

namespace {
nlohmann::json toJson(const ::Account::WalletItem& walletItem) {
  nlohmann::json j;
  j = nlohmann::json{
      {"asset", walletItem.asset}, {"amountFree", walletItem.amountFree}, {"usdtValue", walletItem.usdtValue}};
  return j;
}
}  // namespace

nlohmann::json toJson(const ::Account::Wallet& wallet) {
  nlohmann::json j = nlohmann::json::array();
  for (const auto& walletItem : wallet) {
    const auto walletItemJson = toJson(walletItem);
    j.push_back(walletItemJson);
  }
  return j;
}

}  // namespace GuiService::HttpGuiService::Conversion