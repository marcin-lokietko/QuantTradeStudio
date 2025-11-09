#include "BotBacktester/Simulators/MarketServiceSimulator.hpp"

#include "Utils/ToString.hpp"
#include "spdlog/spdlog.h"

namespace BotBacktester::Simulators {

MarketServiceSimulator::MarketServiceSimulator(
    std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> klines, const Time::ITime& timeSimulator,
    const ApiGateway::TransactionFeePercent transactionFeePercent, ApiGateway::AssetQuantities initialOwnedAssets)
    : klines_(std::move(klines)),
      timeSimulator_(timeSimulator),
      transactionFeePercent_(transactionFeePercent),
      ownedAssets_(std::move(initialOwnedAssets)) {
  updateOwnedAssets();
}

MarketService::AssetValues MarketServiceSimulator::getOwnedAssetValues(
    const ApiGateway::AssetSymbol& quoteAsset) const {
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

ApiGateway::OrderResult MarketServiceSimulator::makeMarketTypeOrderWithQuoteQuantity(
    const ApiGateway::TradingPairSymbol& tradingPairSymbol, const ApiGateway::OrderSide& orderSide,
    const ApiGateway::AssetQuantity& quoteQuantity) const {
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

ApiGateway::Orders MarketServiceSimulator::getOpenOrders() const {
  ApiGateway::Orders orders{};
  SPDLOG_INFO("Simulating getOpenOrders(); returning orders={}", ::toString(orders));
  return orders;
}

ApiGateway::OrderResult MarketServiceSimulator::cancelAllOrdersOnASymbol(
    const ApiGateway::TradingPairSymbol& symbol) const {
  const auto result = ApiGateway::OrderResult::Success;
  SPDLOG_INFO("Simulating cancelAllOrdersOnASymbol(symbol={}); returning={}", toString(symbol), toString(result));
  return result;
}

const BotAssetsHistory& MarketServiceSimulator::getOwnedAssetsHistory() const { return ownedAssetsHistory_; }

void MarketServiceSimulator::updateCurrentKlineIndex() const {
  const auto currentTime = std::chrono::system_clock::from_time_t(timeSimulator_.getTimeSinceEpoch());

  const auto& singleKlineSequence = klines_.begin()->second;

  // Assumes that every received kline sequence has the same (openTime, closeTime) ranges
  while (currentKlineIndex_ + 1 < singleKlineSequence.size() &&
         singleKlineSequence[currentKlineIndex_].closeTime < currentTime) {
    ++currentKlineIndex_;
  }

  throwIfKlinesConsistencyBroken(currentTime);
}

void MarketServiceSimulator::throwIfKlinesConsistencyBroken(
    const std::chrono::system_clock::time_point& currentTime) const {
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

std::optional<double> MarketServiceSimulator::getOneUnitValue(const ApiGateway::TradingPairSymbol& symbol) const {
  const auto it = klines_.find(symbol);
  if (it == klines_.end()) {
    return std::nullopt;
  }
  const auto& klineSequence = it->second;
  return std::stod(klineSequence[currentKlineIndex_].openPrice.val_);
}

void MarketServiceSimulator::updateOwnedAssets() const {
  const auto currentTime = std::chrono::system_clock::from_time_t(timeSimulator_.getTimeSinceEpoch());
  ownedAssetsHistory_.emplace_back(currentTime, ownedAssets_);
}

ApiGateway::SingleAssetQuantity& MarketServiceSimulator::getOrCreateOwnedAsset(
    const ApiGateway::AssetSymbol& symbol) const {
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

}  // namespace BotBacktester::Simulators