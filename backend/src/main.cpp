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

  const std::string filenameLatest = "AlgoTrader_backend_latest.log";
  const std::string filenameWithTime = "AlgoTrader_backend_" + getDatetimeString() + ".log";

  const auto fileSinkLatest =
      std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDir.val_.string() + "/" + filenameLatest, true);
  const auto fileSinkWithTime =
      std::make_shared<spdlog::sinks::basic_file_sink_mt>(logDir.val_.string() + "/" + filenameWithTime, true);

  const std::vector<spdlog::sink_ptr> sinks{fileSinkLatest, fileSinkWithTime};
  const auto logger = std::make_shared<spdlog::logger>("AlgoTrader_backend", sinks.begin(), sinks.end());

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
  MarketService::Binance::BinanceService binanceService{encryption, http, time, config->binanceUrlPrefix};
  BotExecution::BotExecution botExecution{binanceService, binanceService};
  BotBacktester::BotBacktester botBacktester{binanceService};
  ApiGateway::ApiGateway apiGateway{binanceService, botExecution, botBacktester};
  GuiService::HttpGuiService::HttpGuiService guiService(apiGateway);

  guiService.start();
}
