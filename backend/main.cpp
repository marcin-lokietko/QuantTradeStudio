#include <glog/logging.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

#include "GuiService/ServiceFactory.hpp"
#include "StockMarketService/Binance/BinanceService.h"

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

  std::unique_ptr<StockMarketService::IStockMarketService> stockMarketService =
      std::make_unique<StockMarketService::Binance::BinanceService>(argv[2]);

  LOG(INFO) << "Binance testnet server time: " << stockMarketService->getServerTime();

  const std::string symbol = "BTCUSDT";
  LOG(INFO) << "Current price of " << symbol << ": " << stockMarketService->getPrice(symbol);

  // Fetch klines
  const std::string interval = "1h";
  const auto klines = stockMarketService->getKlines(symbol, interval);
  (void)klines;

  // Print Binance testnet account details
  std::string accountResponse = stockMarketService->getAccountData();
  const auto accountJson = nlohmann::json::parse(accountResponse);
  std::ofstream file(std::string(argv[1]) + "/account.txt");
  file << accountJson.dump(4);

  // Make Binance testnet order
  stockMarketService->makeOrder("BTCUSDT", "0.0001", "100000.00");

  auto guiService = GuiService::ServiceFactory().makeGuiService();
  guiService->start();

  LOG(INFO) << "########## Ending AlgoTrader";
}
