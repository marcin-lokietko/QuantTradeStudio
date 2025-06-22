#pragma once
#include <spdlog/spdlog.h>

#include "ApiGateway/TradingPairSymbol.hpp"
#include "IMarketService.hpp"

namespace MarketService {

class TradingPairSymbolDecoderException : public std::exception {
 public:
  explicit TradingPairSymbolDecoderException(std::string message) : message_(std::move(message)) {}

  const char* what() const noexcept override { return ("TradingPairSymbolDecoderException;" + message_).c_str(); }

 private:
  std::string message_;
};

class TradingPairSymbolDecoder {
 public:
  TradingPairSymbolDecoder(MarketService::TradingPairs&& allTradingPairs) { buildMapping(std::move(allTradingPairs)); }

  ApiGateway::TradingPairSymbol decode(const std::string& symbol) const {
    if (const auto it = tradingPairStringToObject_.find(symbol); it != tradingPairStringToObject_.end()) {
      return it->second;
    }
    SPDLOG_ERROR("Trading pair symbol {} not found in mapping", symbol);
    throw TradingPairSymbolDecoderException("Trading pair symbol not found for: " + symbol +
                                            ". Possibly the cached trading data is invalid");
  }

 private:
  void buildMapping(MarketService::TradingPairs&& allTradingPairs) {
    for (const auto& tradingPairSymbol : allTradingPairs) {
      tradingPairStringToObject_[toString(tradingPairSymbol)] = tradingPairSymbol;
    }
  }

  std::map<std::string, ApiGateway::TradingPairSymbol> tradingPairStringToObject_;
};

}  // namespace MarketService