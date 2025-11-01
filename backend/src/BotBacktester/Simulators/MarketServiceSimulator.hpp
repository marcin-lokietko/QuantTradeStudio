#pragma once

#include <chrono>

#include "ApiGateway/BacktestConfig.hpp"
#include "BotBacktester/BotAssetsHistory.hpp"
#include "MarketService/IMarketService.hpp"
#include "MarketService/KlineSequence.hpp"
#include "Utils/Time/ITime.hpp"
#include "Utils/ToString.hpp"
#include "spdlog/spdlog.h"

namespace BotBacktester::Simulators {

class MarketServiceSimulatorException : public std::exception {
 public:
  explicit MarketServiceSimulatorException(std::string message) : message_(std::move(message)) {}

  const char* what() const noexcept override { return ("MarketServiceSimulatorException;" + message_).c_str(); }

 private:
  std::string message_;
};

inline long stringToLong(const std::string& str) {
  try {
    return std::stol(str);
  } catch (const std::invalid_argument& e) {
    throw MarketServiceSimulatorException(
        std::format("Converting string to long failed - invalid input: {}", e.what()));
  } catch (const std::out_of_range& e) {
    throw MarketServiceSimulatorException(std::format("Converting string to long failed - out of range: {}", e.what()));
  }
}

class MarketServiceSimulator : public MarketService::IMarketService {
 public:
  MarketServiceSimulator(std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> klines,
                         const Time::ITime& timeSimulator,
                         const ApiGateway::TransactionFeePercent transactionFeePercent,
                         ApiGateway::AssetQuantities initialOwnedAssets)
      : klines_(std::move(klines)),
        timeSimulator_(timeSimulator),
        transactionFeePercent_(transactionFeePercent),
        ownedAssets_(std::move(initialOwnedAssets)) {
    updateOwnedAssets();
  }

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

  MarketService::AssetValues getOwnedAssetValues(const ApiGateway::AssetSymbol& quoteAsset) const override {
    updateCurrentKlineIndex();

    MarketService::AssetValues assetValues;

    for (const auto& [ownedAssetSymbol, ownedAssetQuantity, _] : ownedAssets_) {
      if (ownedAssetSymbol == quoteAsset) {
        assetValues.emplace_back(ownedAssetSymbol, quoteAsset, ApiGateway::Value{ownedAssetQuantity.val_});
        continue;
      }

      const ApiGateway::TradingPairSymbol tradingPairSymbol{ownedAssetSymbol, quoteAsset};
      const auto oneUnitValue = getOneUnitValue(tradingPairSymbol);
      if (oneUnitValue) {
        const double totalValue = oneUnitValue.value() * std::stod(ownedAssetQuantity.val_);
        assetValues.emplace_back(ownedAssetSymbol, quoteAsset, ApiGateway::Value{std::to_string(totalValue)});
      } else {
        SPDLOG_WARN("No price data available for trading pair: {}", toString(tradingPairSymbol));
      }
    }

    SPDLOG_INFO("Simulating getOwnedAssetValues(quoteAsset={}); returning={}", quoteAsset, ::toString(assetValues));
    return assetValues;
  }

  ApiGateway::OrderResult makeOrder(const ApiGateway::TradingPairSymbol& symbol, const ApiGateway::OrderSide& orderSide,
                                    const ApiGateway::AssetQuantity& quantity,
                                    const ApiGateway::Price& price) override {
    (void)symbol;
    (void)orderSide;
    (void)quantity;
    (void)price;
    throw MarketServiceSimulatorException("Method not implemented: getOwnedAssetsQuantityAndValue");
  }

  ApiGateway::OrderResult makeMarketTypeOrderWithQuoteQuantity(
      const ApiGateway::TradingPairSymbol& tradingPairSymbol, const ApiGateway::OrderSide& orderSide,
      const ApiGateway::AssetQuantity& quoteQuantity) const override {
    updateCurrentKlineIndex();

    const double price = getOneUnitValue(tradingPairSymbol).value();

    const double quoteQuantityDouble = std::stod(quoteQuantity.val_);

    // think: unit of price is how much quote I get for a single base, so [quote/base], so:
    const double baseQuantity = quoteQuantityDouble / price;

    ApiGateway::SingleAssetQuantity& ownedBaseAssetQuantity = getOrCreateOwnedAsset(tradingPairSymbol.baseAsset);
    ApiGateway::SingleAssetQuantity& ownedQuoteAssetQuantity = getOrCreateOwnedAsset(tradingPairSymbol.quoteAsset);

    if (orderSide == ApiGateway::OrderSide::Buy) {
      if (std::stod(ownedQuoteAssetQuantity.freeQuantity.val_) < quoteQuantityDouble) {
        SPDLOG_WARN(
            "Simulated BUY order cannot be fulfilled due to insufficient funds: tradingPairSymbol={}, "
            "quoteQuantity={}, free quote quantity={}",
            toString(tradingPairSymbol), quoteQuantity, ownedQuoteAssetQuantity.freeQuantity);
        return ApiGateway::OrderResult::Failure;
      }
      ownedQuoteAssetQuantity.freeQuantity.val_ =
          std::to_string(std::stod(ownedQuoteAssetQuantity.freeQuantity.val_) - quoteQuantityDouble);

      // Binance charges the fee in the asset you receive when buying/selling
      const auto baseQuantityAfterFee = (baseQuantity * (100.0 - transactionFeePercent_.val_)) / 100.0;
      ownedBaseAssetQuantity.freeQuantity.val_ =
          std::to_string(std::stod(ownedBaseAssetQuantity.freeQuantity.val_) + baseQuantityAfterFee);
    } else if (orderSide == ApiGateway::OrderSide::Sell) {
      if (std::stod(ownedBaseAssetQuantity.freeQuantity.val_) < baseQuantity) {
        SPDLOG_WARN(
            "Simulated SELL order cannot be fulfilled due to insufficient funds: tradingPairSymbol={}, "
            "baseQuantity={}, free base quantity={}",
            toString(tradingPairSymbol), baseQuantity, ownedBaseAssetQuantity.freeQuantity);
        return ApiGateway::OrderResult::Failure;
      }
      ownedBaseAssetQuantity.freeQuantity.val_ =
          std::to_string(std::stod(ownedBaseAssetQuantity.freeQuantity.val_) - baseQuantity);

      const auto quoteQuantityDoubleAfterFee = (quoteQuantityDouble * (100.0 - transactionFeePercent_.val_)) / 100.0;
      ownedQuoteAssetQuantity.freeQuantity.val_ =
          std::to_string(std::stod(ownedQuoteAssetQuantity.freeQuantity.val_) + quoteQuantityDoubleAfterFee);
    }

    updateOwnedAssets();

    SPDLOG_INFO(
        "Successfully simulating makeMarketTypeOrderWithQuoteQuantity(tradingPairSymbol={}, orderSide={}, "
        "quoteQuantity={})",
        toString(tradingPairSymbol), toString(orderSide), quoteQuantity);
    return ApiGateway::OrderResult::Success;
  }

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

  ApiGateway::Orders getOpenOrders() const override {
    ApiGateway::Orders orders{};
    SPDLOG_INFO("Simulating getOpenOrders(); returning orders={}", ::toString(orders));
    return orders;
  }

  ApiGateway::OrderResult cancelAllOrdersOnASymbol(const ApiGateway::TradingPairSymbol& symbol) const override {
    const auto result = ApiGateway::OrderResult::Success;
    SPDLOG_INFO("Simulating cancelAllOrdersOnASymbol(symbol={}); returning={}", toString(symbol), toString(result));
    return result;
  }

  const BotAssetsHistory& getOwnedAssetsHistory() const { return ownedAssetsHistory_; }

 private:
  void updateCurrentKlineIndex() const {
    const auto currentTime = std::chrono::system_clock::from_time_t(timeSimulator_.getTimeSinceEpoch());

    const auto& singleKlineSequence = klines_.begin()->second;

    // Assumes that every received kline sequence has the same (openTime, closeTime) ranges
    while (currentKlineIndex_ + 1 < singleKlineSequence.size() &&
           singleKlineSequence[currentKlineIndex_].closeTime < currentTime) {
      ++currentKlineIndex_;
    }

    throwIfKlinesConsistencyBroken(currentTime);
  }

  void throwIfKlinesConsistencyBroken(const std::chrono::system_clock::time_point& currentTime) const {
    for (const auto& [symbol, klines] : klines_) {
      if (currentKlineIndex_ >= klines.size()) {
        throw MarketServiceSimulatorException{std::format(
            "Klines consistency broken: currentKlineIndex_ exceeds size of klines for symbol={}", toString(symbol))};
      }
      if (klines[currentKlineIndex_].openTime > currentTime || klines[currentKlineIndex_].closeTime < currentTime) {
        throw MarketServiceSimulatorException{std::format(
            "Klines consistency broken: [openTime, closeTime] for symbol={} at index={} does not contain current time",
            toString(symbol), currentKlineIndex_)};
      }
    }
  }

  std::optional<double> getOneUnitValue(const ApiGateway::TradingPairSymbol& symbol) const {
    const auto it = klines_.find(symbol);
    if (it == klines_.end()) {
      return std::nullopt;
    }
    const auto& klineSequence = it->second;
    return std::stod(klineSequence[currentKlineIndex_].openPrice.val_);
  }

  void updateOwnedAssets() const {
    const auto currentTime = std::chrono::system_clock::from_time_t(timeSimulator_.getTimeSinceEpoch());
    ownedAssetsHistory_.emplace_back(currentTime, ownedAssets_);
  }

  ApiGateway::SingleAssetQuantity& getOrCreateOwnedAsset(const ApiGateway::AssetSymbol& symbol) const {
    auto it =
        std::find_if(ownedAssets_.begin(), ownedAssets_.end(),
                     [&symbol](const ApiGateway::SingleAssetQuantity& asset) { return asset.assetSymbol == symbol; });
    if (it != ownedAssets_.end()) {
      return *it;
    } else {
      ownedAssets_.emplace_back(symbol, ApiGateway::AssetQuantity{"0"}, ApiGateway::MarketId::Unknown);
      return ownedAssets_.back();
    }
  }

  mutable size_t currentKlineIndex_{0};

  const std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> klines_;
  const Time::ITime& timeSimulator_;
  const ApiGateway::TransactionFeePercent transactionFeePercent_{};
  mutable ApiGateway::AssetQuantities ownedAssets_{};
  mutable BotAssetsHistory ownedAssetsHistory_{};
};

}  // namespace BotBacktester::Simulators