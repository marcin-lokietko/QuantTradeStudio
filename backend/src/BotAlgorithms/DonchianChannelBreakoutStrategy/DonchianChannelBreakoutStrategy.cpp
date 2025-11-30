#include "DonchianChannelBreakoutStrategy.hpp"

#include <spdlog/spdlog.h>

#include <thread>

#include "BotAlgorithms/Utils.hpp"

namespace BotAlgorithms::DonchianChannelBreakoutStrategy {

DonchianChannelBreakoutStrategy::DonchianChannelBreakoutStrategy(
    Config&& config, const MarketService::IMarketService& marketService,
    const MarketService::IHistoricalMarketDataProvider& historicalMarketDataProvider, const Time::ITime& time)
    : config_(std::move(config)),
      marketService_(marketService),
      historicalMarketDataProvider_(historicalMarketDataProvider),
      time_(time) {
  for (const auto& assetSymbol : config_.baseAssets) {
    detectors_.emplace(assetSymbol, BreakoutDetector(config_.entryChannelLength, config_.exitChannelLength));
  }
}

void DonchianChannelBreakoutStrategy::run(std::stop_token st) {
  SPDLOG_INFO("DonchianChannelBreakoutStrategy started execution");
  const auto executionPeriod = toMilliseconds(config_.executionInterval);
  while (!st.stop_requested()) {
    SPDLOG_INFO("DonchianChannelBreakoutStrategy sleeping for {}", executionPeriod.count());
    time_.sleepFor(st, executionPeriod);
    if (!st.stop_requested()) {
      performIteration();
    }
  }
  SPDLOG_INFO("DonchianChannelBreakoutStrategy stopped execution");
}

void DonchianChannelBreakoutStrategy::performIteration() {
  SPDLOG_INFO("Performing DonchianChannelBreakoutStrategy iteration");
  processKlinesForAllDetectors();
  placeOrdersBasedOnDetectorSignals();
}

void DonchianChannelBreakoutStrategy::processKlinesForAllDetectors() {
  for (auto& [assetSymbol, detector] : detectors_) {
    const ApiGateway::TradingPairSymbol tradingPairSymbol{assetSymbol, config_.quoteAsset};

    const uint64_t maxChannelLength = std::max(config_.entryChannelLength.val_, config_.exitChannelLength.val_);
    const auto endTime = std::chrono::system_clock::from_time_t(time_.getTimeSinceEpoch());
    const std::chrono::system_clock::time_point startTime =
        endTime - (toMilliseconds(config_.executionInterval) * maxChannelLength);

    const auto klines =
        historicalMarketDataProvider_.getKlines(tradingPairSymbol, config_.executionInterval, startTime, endTime);

    if (klines.size() < maxChannelLength) {
      SPDLOG_ERROR("Not enough klines to process for asset {}. Required: {}, available: {}", assetSymbol.val_,
                   maxChannelLength, klines.size());
    } else {
      detector.processKlines(klines);
    }
  }
}

// TODO: partially duplicates MovingAverageCrossover::placeOrdersBasedOnCrossoverSignals
void DonchianChannelBreakoutStrategy::placeOrdersBasedOnDetectorSignals() {
  std::unique_ptr<ApiGateway::AssetQuantitiesMap> ownedAssetsAmounts = nullptr;
  const auto cancelOrdersAndInitOwnedAssetsAmounts = [&ownedAssetsAmounts, &marketService = marketService_,
                                                      &config = config_]() {
    cancelOpenOrders(marketService, config.quoteAsset, config.baseAssets);
    if (!ownedAssetsAmounts) {
      const auto ownedAssetsVector = marketService.getOwnedAssetsQuantity();
      ownedAssetsAmounts = std::make_unique<ApiGateway::AssetQuantitiesMap>(ApiGateway::asMap(ownedAssetsVector));
    }
  };

  for (auto& [assetSymbol, detector] : detectors_) {
    const ApiGateway::TradingPairSymbol tradingPairSymbol{assetSymbol, config_.quoteAsset};

    if (detector.isBuySignalled()) {
      SPDLOG_INFO("Buy signal detected for asset {}", assetSymbol.val_);
      cancelOrdersAndInitOwnedAssetsAmounts();
      if (ownedAssetsAmounts->contains(config_.quoteAsset)) {
        SPDLOG_INFO("Decided to buy {} for all available {}", assetSymbol, config_.quoteAsset);
        marketService_.makeMarketTypeOrderWithQuoteQuantity(tradingPairSymbol, ApiGateway::OrderSide::Buy,
                                                            ownedAssetsAmounts->at(config_.quoteAsset));
      }
    } else if (detector.isSellSignalled()) {
      SPDLOG_INFO("Sell signal detected for asset {}", assetSymbol.val_);
      cancelOrdersAndInitOwnedAssetsAmounts();
      if (ownedAssetsAmounts->contains(assetSymbol)) {
        SPDLOG_INFO("Decided to sell all available {}", assetSymbol);
        marketService_.makeOrder(tradingPairSymbol, ApiGateway::OrderSide::Sell, ownedAssetsAmounts->at(assetSymbol),
                                 std::nullopt);
      }
    }
  }
}
}  // namespace BotAlgorithms::DonchianChannelBreakoutStrategy