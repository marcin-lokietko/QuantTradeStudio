#include <glog/logging.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

#include "ApiGateway/ApiGateway.hpp"
#include "BotExecution/BotExecution.hpp"
#include "Config/ReadConfig.hpp"
#include "GuiService/HttpGuiService/HttpGuiService.hpp"
#include "MarketService/Binance/BinanceService.hpp"
#include "Wallet/Wallet.hpp"

void setupLogger(const char* programName, const Config::LogsCatalogPath& logDir) {
  if (!std::filesystem::exists(logDir.val_)) {
    std::filesystem::create_directory(logDir.val_);
  }
  FLAGS_log_dir = logDir.val_;
  FLAGS_stderrthreshold = 0;

  // always flush:
  FLAGS_logbuflevel = -1;
  FLAGS_logbufsecs = 0;

  google::InitGoogleLogging(programName);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Invalid arguments; argc=" << argc;
    return 1;
  }
  const auto config = Config::readConfig(argv[1]);
  if (!config) {
    std::cerr << "Invalid configuration";
    return 2;
  }

  setupLogger(argv[0], config->logsCatalogPath);

  LOG(INFO) << "########## Config read; starting AlgoTrader";

  MarketService::Binance::BinanceService marketService{config->keysCatalogPath, config->binanceUrlPrefix};
  Wallet::Wallet wallet{marketService};
  BotExecution::BotExecution botExecution{marketService, wallet};
  ApiGateway::ApiGateway apiGateway{marketService, wallet, botExecution};
  GuiService::HttpGuiService::HttpGuiService guiService(apiGateway);
  guiService.start();
}
