#pragma once

#include <spdlog/spdlog.h>

#include <chrono>

#include "ApiGateway/BacktestConfig.hpp"
#include "MarketService/IHistoricalMarketDataProvider.hpp"
#include "MarketService/KlineSequence.hpp"

namespace BotBacktester::Simulators {

class HistoricalMarketDataProviderSimulatorException : public std::exception {
 public:
  explicit HistoricalMarketDataProviderSimulatorException(std::string message) : message_(std::move(message)) {}

  const char* what() const noexcept override {
    return ("HistoricalMarketDataProviderSimulatorException;" + message_).c_str();
  }

 private:
  std::string message_;
};

class HistoricalMarketDataProviderSimulator : public MarketService::IHistoricalMarketDataProvider {
 public:
  HistoricalMarketDataProviderSimulator(std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> klines);

  // this approximates klines with requested klineInterval based on kilnes_ requested during construction
  virtual MarketService::KlineSequence getKlines(const ApiGateway::TradingPairSymbol& symbol,
                                                 const ApiGateway::KlineInterval klineInterval,
                                                 const std::chrono::system_clock::time_point startTime,
                                                 const std::chrono::system_clock::time_point endTime) const;

 private:
  const MarketService::KlineSequence& getKlineSequence(const ApiGateway::TradingPairSymbol& symbol) const;

  void throwIfSourceKlinesNotSuitableRequestedTimeRange(const MarketService::KlineSequence& sourceKlines,
                                                        const ApiGateway::KlineInterval requestedKlineInterval,
                                                        const std::chrono::system_clock::time_point startTime,
                                                        const std::chrono::system_clock::time_point endTime) const;

  const std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> klines_;
};

}  // namespace BotBacktester::Simulators