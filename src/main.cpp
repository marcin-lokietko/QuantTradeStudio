#include <glog/logging.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>

#include "StockMarketService/BinanceService.h"

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
      std::make_unique<StockMarketService::BinanceService>(argv[2]);

  LOG(INFO) << "Binance testnet GET /api/v3/time response: " << stockMarketService->getServerTime();

  const std::string symbol = "BTCUSDT";
  LOG(INFO) << "Current price of " << symbol << ": " << stockMarketService->getPrice(symbol);

  // Fetch klines
  const std::string interval = "1h";
  const std::string klinesResponse = stockMarketService->getKlines(symbol, interval);
  const auto klinesJson = nlohmann::json::parse(klinesResponse);
  std::ofstream fileKlines(std::string(argv[1]) + "/klines.txt");
  fileKlines << klinesJson.dump(4);

  // Print Binance testnet account details
  std::string accountResponse = stockMarketService->getAccountData();
  const auto accountJson = nlohmann::json::parse(accountResponse);
  std::ofstream file(std::string(argv[1]) + "/account.txt");
  file << accountJson.dump(4);

  // Make Binance testnet order
  stockMarketService->makeOrder();

  LOG(INFO) << "########## Ending AlgoTrader";
}
