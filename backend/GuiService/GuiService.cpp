#include <glog/logging.h>

#include <nlohmann/json.hpp>

#include "Account/Wallet.h"
#include "GuiService.hpp"
#include "crow.h"

namespace GuiService {

void to_json(nlohmann::json& j, const Account::WalletItem& walletItem) {
  j = nlohmann::json{
      {"asset", walletItem.asset}, {"amountFree", walletItem.amountFree}, {"usdtValue", walletItem.usdtValue}};
}

void GuiService::start() {
  crow::SimpleApp app;

  CROW_ROUTE(app, "/hello")
  ([]() {
    crow::response res("Hello world from backend");
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Content-Type", "application/json");
    return res;
  });

  CROW_ROUTE(app, "/wallet")
  ([&account_ = account_]() {
    const auto wallet = account_.getWallet();
    nlohmann::json walletJson = nlohmann::json::array();
    for (const auto& walletItem : wallet) {
      nlohmann::json walletItemJson;
      to_json(walletItemJson, walletItem);
      walletJson.push_back(walletItemJson);
    }

    // TODO merge wallet with prices and send it to frontend
    crow::response res(walletJson.dump());
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Content-Type", "application/json");
    return res;
  });

  LOG(INFO) << "Starting service";
  app.port(5000).run();
}

}  // namespace GuiService