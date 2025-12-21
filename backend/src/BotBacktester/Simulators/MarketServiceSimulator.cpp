#include "BotBacktester/Simulators/MarketServiceSimulator.hpp"

#include "BotBacktester/Utils.hpp"
#include "Utils/ToString.hpp"

namespace BotBacktester::Simulators {

MarketServiceSimulator::MarketServiceSimulator(
    std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> klines, const Time::ITime& timeSimulator,
    const ApiGateway::TransactionFeePercent transactionFeePercent, ApiGateway::AssetQuantities initialOwnedAssets)
    : klines_(std::move(klines)),
      timeSimulator_(timeSimulator),
      transactionFeePercent_(transactionFeePercent),
      ownedAssets_(std::move(initialOwnedAssets)) {
  if (doKlinesHaveDifferentSizesOrTimeRanges(klines_)) {
    throw MarketServiceSimulatorException(
        "Contract broken - klines provided to MarketServiceSimulator have different sizes or time ranges");
  }
  updateOwnedAssets();
}

MarketService::AssetPrices MarketServiceSimulator::getPrices(
    const std::vector<ApiGateway::TradingPairSymbol>& symbols) const {
  updateCurrentKlineIndex();

  MarketService::AssetPrices assetPrices;
  for (const auto& singleSymbol : symbols) {
    const auto unitValue = getCurrentValueOfUnit(singleSymbol);
    if (unitValue) {
      assetPrices.emplace_back(singleSymbol, ApiGateway::Price{std::to_string(unitValue.value())});
    } else {
      SPDLOG_ERROR("No price data available for trading pair: {}", toString(singleSymbol));
    }
  }
  return assetPrices;
}

ApiGateway::AssetQuantities MarketServiceSimulator::getOwnedAssetsQuantity() const { return ownedAssets_; }

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
    const auto unitValue = getCurrentValueOfUnit(tradingPairSymbol);
    if (unitValue) {
      const double totalValue = unitValue.value() * std::stod(ownedAssetQuantity.val_);
      assetValues.emplace_back(ownedAssetSymbol, quoteAsset, ApiGateway::Value{std::to_string(totalValue)});
    } else {
      SPDLOG_WARN("No price data available for trading pair: {}", toString(tradingPairSymbol));
    }
  }

  SPDLOG_INFO("Simulating getOwnedAssetValues(quoteAsset={}); returning={}", quoteAsset, ::toString(assetValues));
  return assetValues;
}

ApiGateway::OrderResult MarketServiceSimulator::makeOrder(const ApiGateway::TradingPairSymbol& symbol,
                                                          const ApiGateway::OrderSide& orderSide,
                                                          const ApiGateway::AssetQuantity& quantity,
                                                          const std::optional<ApiGateway::Price>& price) const {
  if (price.has_value()) {
    throw MarketServiceSimulatorException("Method not implemented: makeOrder with LIMIT price");
  }

  updateCurrentKlineIndex();

  const double unitValue = getCurrentValueOfUnit(symbol).value();
  const double baseQuantityDouble = std::stod(quantity.val_);
  // value = quote / base  =>  quote = value * base
  const double quoteQuantityDouble = unitValue * baseQuantityDouble;

  return simulateOrder(symbol, orderSide, quoteQuantityDouble, baseQuantityDouble);
}

ApiGateway::OrderResult MarketServiceSimulator::makeMarketTypeOrderWithQuoteQuantity(
    const ApiGateway::TradingPairSymbol& tradingPairSymbol, const ApiGateway::OrderSide& orderSide,
    const ApiGateway::AssetQuantity& quoteQuantity) const {
  updateCurrentKlineIndex();

  const double unitValue = getCurrentValueOfUnit(tradingPairSymbol).value();
  const double quoteQuantityDouble = std::stod(quoteQuantity.val_);
  const double baseQuantityDouble = quoteQuantityDouble / unitValue;

  const auto result = simulateOrder(tradingPairSymbol, orderSide, quoteQuantityDouble, baseQuantityDouble);

  if (result == ApiGateway::OrderResult::Success) {
    SPDLOG_INFO(
        "Successfully simulating makeMarketTypeOrderWithQuoteQuantity(tradingPairSymbol={}, orderSide={}, "
        "quoteQuantity={})",
        toString(tradingPairSymbol), toString(orderSide), quoteQuantity);
  }
  return result;
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

ApiGateway::OrderResult MarketServiceSimulator::simulateOrder(const ApiGateway::TradingPairSymbol& tradingPairSymbol,
                                                              const ApiGateway::OrderSide& orderSide,
                                                              const double quoteQuantity,
                                                              const double baseQuantity) const {
  ApiGateway::SingleAssetQuantity& ownedBaseAssetQuantity = getOrCreateOwnedAsset(tradingPairSymbol.baseAsset);
  ApiGateway::SingleAssetQuantity& ownedQuoteAssetQuantity = getOrCreateOwnedAsset(tradingPairSymbol.quoteAsset);

  if (orderSide == ApiGateway::OrderSide::Buy) {
    if (std::stod(ownedQuoteAssetQuantity.freeQuantity.val_) < quoteQuantity) {
      SPDLOG_WARN(
          "Simulated BUY order cannot be fulfilled due to insufficient funds: tradingPairSymbol={}, "
          "quoteQuantity={}, free quote quantity={}",
          toString(tradingPairSymbol), quoteQuantity, ownedQuoteAssetQuantity.freeQuantity);
      return ApiGateway::OrderResult::Failure;
    }
    ownedQuoteAssetQuantity.freeQuantity.val_ =
        std::to_string(std::stod(ownedQuoteAssetQuantity.freeQuantity.val_) - quoteQuantity);
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
    const auto quoteQuantityAfterFee = (quoteQuantity * (100.0 - transactionFeePercent_.val_)) / 100.0;
    ownedQuoteAssetQuantity.freeQuantity.val_ =
        std::to_string(std::stod(ownedQuoteAssetQuantity.freeQuantity.val_) + quoteQuantityAfterFee);
  }

  updateOwnedAssets();
  return ApiGateway::OrderResult::Success;
}

void MarketServiceSimulator::updateCurrentKlineIndex() const {
  const auto currentTime = std::chrono::system_clock::from_time_t(timeSimulator_.getTimeSinceEpoch());

  const auto& firstKlineSequence = klines_.begin()->second;

  while (currentKlineIndex_ + 1 < firstKlineSequence.size() &&
         firstKlineSequence[currentKlineIndex_].closeTime < currentTime) {
    ++currentKlineIndex_;
  }
}

std::optional<double> MarketServiceSimulator::getCurrentValueOfUnit(const ApiGateway::TradingPairSymbol& symbol) const {
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
  const auto it =
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