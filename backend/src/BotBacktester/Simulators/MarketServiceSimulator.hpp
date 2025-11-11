#pragma once

#include <chrono>

#include "ApiGateway/BacktestConfig.hpp"
#include "BotBacktester/BotAssetsHistory.hpp"
#include "MarketService/IMarketService.hpp"
#include "MarketService/KlineSequence.hpp"
#include "Utils/Time/ITime.hpp"

namespace BotBacktester::Simulators {

class MarketServiceSimulatorException : public std::exception {
 public:
  explicit MarketServiceSimulatorException(std::string message) : message_(std::move(message)) {}

  const char* what() const noexcept override { return ("MarketServiceSimulatorException;" + message_).c_str(); }

 private:
  std::string message_;
};

class MarketServiceSimulator : public MarketService::IMarketService {
 public:
  MarketServiceSimulator(std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> klines,
                         const Time::ITime& timeSimulator,
                         const ApiGateway::TransactionFeePercent transactionFeePercent,
                         ApiGateway::AssetQuantities initialOwnedAssets);

  MarketService::Time getServerTime() override {
    throw MarketServiceSimulatorException("Method not implemented: getServerTime");
  }

  ApiGateway::Price getPrice(const ApiGateway::TradingPairSymbol& tradingPairSymbol) override {
    (void)tradingPairSymbol;
    throw MarketServiceSimulatorException("Method not implemented: getPrice");
  }

  MarketService::AssetPrices getPrices(const std::vector<ApiGateway::TradingPairSymbol>& symbols) const {
    (void)symbols;
    throw MarketServiceSimulatorException("Method not implemented: getPrices");
  }

  ApiGateway::AssetQuantities getOwnedAssetsQuantity() const override {
    throw MarketServiceSimulatorException("Method not implemented: getOwnedAssetsQuantity");
  }

  ApiGateway::AssetValues getOwnedAssetsQuantityAndValue() const override {
    throw MarketServiceSimulatorException("Method not implemented: getOwnedAssetsQuantityAndValue");
  }

  MarketService::AssetValues getOwnedAssetValues(const ApiGateway::AssetSymbol& quoteAsset) const;

  ApiGateway::OrderResult makeOrder(const ApiGateway::TradingPairSymbol& symbol, const ApiGateway::OrderSide& orderSide,
                                    const ApiGateway::AssetQuantity& quantity,
                                    const ApiGateway::Price& price) override {
    (void)symbol;
    (void)orderSide;
    (void)quantity;
    (void)price;
    throw MarketServiceSimulatorException("Method not implemented: getOwnedAssetsQuantityAndValue");
  }

  ApiGateway::OrderResult makeMarketTypeOrderWithQuoteQuantity(const ApiGateway::TradingPairSymbol& tradingPairSymbol,
                                                               const ApiGateway::OrderSide& orderSide,
                                                               const ApiGateway::AssetQuantity& quoteQuantity) const;

  MarketService::TradingPairs getAllTradingPairs() const override {
    throw MarketServiceSimulatorException("Method not implemented: getAllTradingPairs");
  }

  MarketService::TradingPairs getTradingPairsWithQuoteAsset(const ApiGateway::AssetSymbol& quoteAsset) const override {
    (void)quoteAsset;
    throw MarketServiceSimulatorException("Method not implemented: getTradingPairsWithQuoteAsset");
  }

  MarketService::TradingPairs getTradingPairsWithBaseAsset(const ApiGateway::AssetSymbol& baseAsset) const override {
    (void)baseAsset;
    throw MarketServiceSimulatorException("Method not implemented: getTradingPairsWithBaseAsset");
  }

  ApiGateway::Orders getOpenOrders() const;

  ApiGateway::OrderResult cancelAllOrdersOnASymbol(const ApiGateway::TradingPairSymbol& symbol) const;

  const BotAssetsHistory& getOwnedAssetsHistory() const;

 private:
  void updateCurrentKlineIndex() const;

  std::optional<double> getCurrentValueOfUnit(const ApiGateway::TradingPairSymbol& symbol) const;

  void updateOwnedAssets() const;

  ApiGateway::SingleAssetQuantity& getOrCreateOwnedAsset(const ApiGateway::AssetSymbol& symbol) const;

  mutable size_t currentKlineIndex_{0};

  const std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> klines_;
  const Time::ITime& timeSimulator_;
  const ApiGateway::TransactionFeePercent transactionFeePercent_{};
  mutable ApiGateway::AssetQuantities ownedAssets_{};
  mutable BotAssetsHistory ownedAssetsHistory_{};
};

}  // namespace BotBacktester::Simulators