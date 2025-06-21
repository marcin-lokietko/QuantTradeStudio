#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

#include "ApiGateway/ApiGateway.hpp"
#include "BotExecution/BotExecution.hpp"
#include "Config/ReadConfig.hpp"
#include "GuiService/HttpGuiService/HttpGuiService.hpp"
#include "Http/Http.hpp"
#include "MarketService/Binance/BinanceService.hpp"
#include "MarketService/Binance/Encryption.hpp"
#include "Utils/Time/Time.hpp"
#include "Wallet/Wallet.hpp"

std::string get_datetime_string() {
  auto now = std::chrono::system_clock::now();
  std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
  std::tm tm;
  localtime_r(&nowTime, &tm);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
  return oss.str();
}

void setupLogger(const Config::LogsCatalogPath& logDir) {
  if (!std::filesystem::exists(logDir.val_)) {
    std::filesystem::create_directory(logDir.val_);
  }

  const std::string filename = "AlgoTrader_backend_" + get_datetime_string() + ".log";
  auto logger = spdlog::basic_logger_mt("AlgoTrader_backend", logDir.val_.string() + "/" + filename);
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::trace);
  spdlog::flush_on(spdlog::level::trace);
  spdlog::set_pattern("[%s:%#] [%l] %v");  // Set the log pattern to include source file and line number
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
  const Time::Time time{};
  MarketService::Binance::BinanceService marketService{encryption, http, time, config->binanceUrlPrefix};
  Wallet::Wallet wallet{marketService};
  BotExecution::BotExecution botExecution{marketService, wallet};
  ApiGateway::ApiGateway apiGateway{marketService, wallet, botExecution};
  GuiService::HttpGuiService::HttpGuiService guiService(apiGateway);
  guiService.start();
}
