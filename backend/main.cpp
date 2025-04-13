#include <glog/logging.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

#include "ApiGateway/ApiGateway.hpp"
#include "BotExecution/BotExecution.hpp"
#include "GuiService/HttpGuiService/HttpGuiService.hpp"
#include "MarketService/Binance/BinanceService.hpp"
#include "Wallet/Wallet.hpp"

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

  MarketService::Binance::BinanceService marketService{argv[2]};
  Wallet::Wallet wallet{marketService};
  BotExecution::BotExecution botExecution{marketService};
  ApiGateway::ApiGateway apiGateway{marketService, wallet, botExecution};
  GuiService::HttpGuiService::HttpGuiService guiService(apiGateway);
  guiService.start();

  LOG(INFO) << "########## Ending AlgoTrader";
}
