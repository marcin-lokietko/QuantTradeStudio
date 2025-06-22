#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

#include "ApiGateway/ApiGateway.hpp"
#include "BotBacktester/BotBacktester.hpp"
#include "BotExecution/BotExecution.hpp"
#include "Config/ReadConfig.hpp"
#include "GuiService/HttpGuiService/HttpGuiService.hpp"
#include "Http/Http.hpp"
#include "MarketService/Binance/BinanceService.hpp"
#include "MarketService/Binance/Encryption.hpp"
#include "Utils/Time/SystemTime.hpp"

std::string getDatetimeString() {
  auto now = std::chrono::system_clock::now();
  std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
  std::tm tm;
  localtime_r(&nowTime, &tm);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S-UTC0");
  return oss.str();
}

void setupLogger(const Config::LogsCatalogPath& logDir) {
  if (!std::filesystem::exists(logDir.val_)) {
    std::filesystem::create_directory(logDir.val_);
  }

  const std::string filename = "AlgoTrader_backend_" + getDatetimeString() + ".log";
  auto logger = spdlog::basic_logger_mt("AlgoTrader_backend", logDir.val_.string() + "/" + filename);
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::trace);
  spdlog::flush_on(spdlog::level::trace);
  // Log pattern: [time] [file:line] [level] message
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e UTC+0] [%s:%#] [%l] %v");
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

  setupLogger(config->logsCatalogPath);

  SPDLOG_INFO("########## Config read; starting AlgoTrader");

  const MarketService::Binance::Encryption encryption{config->keysCatalogPath};
  const Http::Http http{};
  const Time::SystemTime time{};
  MarketService::Binance::BinanceService marketService{encryption, http, time, config->binanceUrlPrefix};
  BotExecution::BotExecution botExecution{marketService};
  BotBacktester::BotBacktester botBacktester{marketService};
  ApiGateway::ApiGateway apiGateway{marketService, botExecution, botBacktester};
  GuiService::HttpGuiService::HttpGuiService guiService(apiGateway);

  // const ApiGateway::BotConfig botConfig{
  //     ApiGateway::BotName{"Rebalancer"}, std::make_optional<ApiGateway::ExecutionPeriod>(60),
  //     std::make_optional<ApiGateway::IsExecutedImmediately>(true),
  //     std::make_optional<ApiGateway::AssetSymbol>("USDT"), ApiGateway::AssetShares{
  //         ApiGateway::SingleAssetShare{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::SharePercent{80}},
  //         ApiGateway::SingleAssetShare{ApiGateway::AssetSymbol{"ETH"}, ApiGateway::SharePercent{20}}}};

  // using namespace std::chrono;

  // const ApiGateway::BacktesterConfig backtesterConfig{
  //     ApiGateway::TransactionFeePercent{0.001},
  //     ApiGateway::AssetQuantities{{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"0.1"}},
  //                                 {ApiGateway::AssetSymbol{"ETH"}, ApiGateway::AssetQuantity{"1.0"}},
  //                                 {ApiGateway::AssetSymbol{"USDT"}, ApiGateway::AssetQuantity{"10000.5"}}},
  //     std::chrono::system_clock::time_point{sys_days{2025y / August / 10d} + 12h},
  //     std::chrono::system_clock::time_point{sys_days{2025y / August / 10d} + 13h}};

  // botBacktester.testBot(botConfig, backtesterConfig);

  guiService.start();
}
