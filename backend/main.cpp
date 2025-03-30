#include <glog/logging.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

#include "Account/Account.hpp"
#include "GuiService/HttpGuiService/HttpGuiService.hpp"
#include "MarketService/Binance/BinanceService.hpp"

void setupLogger(const char* programName, const char* logDir) {
  std::filesystem::create_directory(logDir);
  FLAGS_log_dir = logDir;
  FLAGS_stderrthreshold = 0;

  google::InitGoogleLogging(programName);

  LOG(INFO) << "Glog logger successfully initialized";
}

int main(int, char* argv[]) {
  setupLogger(argv[0], argv[1]);

  LOG(INFO) << "########## Starting AlgoTrader";

  std::unique_ptr<MarketService::IMarketService> marketService =
      std::make_unique<MarketService::Binance::BinanceService>(argv[2]);

  std::unique_ptr<Account::IAccount> account = std::make_unique<Account::Account>(*marketService.get());

  std::unique_ptr<GuiService::IGuiService> guiService =
      std::make_unique<GuiService::HttpGuiService::HttpGuiService>(*account.get());
  guiService->start();

  LOG(INFO) << "########## Ending AlgoTrader";
}
