#include "MovingAverageCrossover.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <memory>
#include <ranges>
#include <set>
#include <thread>
#include <vector>

#include "ApiGateway/TradingPairSymbol.hpp"
#include "BotAlgorithms/Utils.hpp"

namespace BotAlgorithms::MovingAverageCrossover {

MovingAverageCrossover::MovingAverageCrossover(Config&& config, const MarketService::IMarketService& marketService,
                                               const Time::ITime& time)
    : config_(std::move(config)), marketService_(marketService), time_(time) {
  for (const auto& assetSymbol : config_.baseAssets) {
    assetHistories_.emplace(assetSymbol,
                            AssetHistory(config_.shortTermMovingAverageLength, config_.longTermMovingAverageLength));
  }
}

void MovingAverageCrossover::run(std::stop_token st) {
  SPDLOG_INFO("MovingAverageCrossover started execution");
  const std::chrono::seconds executionPeriod(config_.executionPeriod.val_);

  while (!st.stop_requested()) {
    SPDLOG_INFO("MovingAverageCrossover sleeping for {}", executionPeriod.count());
    time_.sleepFor(st, executionPeriod);
    if (!st.stop_requested()) {
      performIteration();
    }
  }
  SPDLOG_INFO("MovingAverageCrossover stopped execution");
}

void MovingAverageCrossover::performIteration() {
  SPDLOG_INFO("Performing Moving Average Crossover iteration");
  addHistoryForAllAssets();
  if (isMinimalHistoryCollectedForAllAssets()) {
    placeOrdersBasedOnCrossoverSignals();
  } else {
    SPDLOG_INFO("Minimal history not yet collected for all assets, skipping checking the buy/sell signals");
  }
}

void MovingAverageCrossover::addHistoryForAllAssets() {
  auto assetPairsView = config_.baseAssets | std::views::transform([&](const auto& assetSymbol) {
                          return ApiGateway::TradingPairSymbol{assetSymbol, config_.quoteAsset};
                        });
  const auto prices = marketService_.getPrices(
      std::vector<ApiGateway::TradingPairSymbol>(assetPairsView.begin(), assetPairsView.end()));

  for (const auto& [tradingPairSymbol, price] : prices) {
    SPDLOG_INFO("Adding price {} to price history of {}", toString(price), toString(tradingPairSymbol));
    const auto& assetSymbol = tradingPairSymbol.baseAsset;
    auto& assetHistory = assetHistories_.at(assetSymbol);
    assetHistory.addValue(price);
  }
}

bool MovingAverageCrossover::isMinimalHistoryCollectedForAllAssets() const {
  return std::ranges::all_of(assetHistories_,
                             [](const auto& assetHistory) { return assetHistory.second.isMinimalHistoryCollected(); });
}

void MovingAverageCrossover::placeOrdersBasedOnCrossoverSignals() {
  std::unique_ptr<ApiGateway::AssetQuantitiesMap> ownedAssetsAmounts = nullptr;
  const auto cancelOrdersAndInitOwnedAssetsAmounts = [&ownedAssetsAmounts, &marketService = marketService_,
                                                      &config = config_]() {
    cancelOpenOrders(marketService, config.quoteAsset, config.baseAssets);
    SPDLOG_INFO("Cancelled all relevant open orders");
    if (!ownedAssetsAmounts) {
      SPDLOG_INFO("Fetching owned assets quantities");
      const auto ownedAssetsVector = marketService.getOwnedAssetsQuantity();
      ownedAssetsAmounts = std::make_unique<ApiGateway::AssetQuantitiesMap>(ApiGateway::asMap(ownedAssetsVector));
    }
  };

  for (auto& [assetSymbol, assetHistory] : assetHistories_) {
    SPDLOG_INFO("Checking the buy/sell signals for {}", toString(assetSymbol));

    const ApiGateway::TradingPairSymbol tradingPairSymbol{assetSymbol, config_.quoteAsset};

    if (assetHistory.isBuySignalled()) {
      SPDLOG_INFO("Buy signal detected for asset {}", assetSymbol.val_);
      cancelOrdersAndInitOwnedAssetsAmounts();
      if (ownedAssetsAmounts->contains(config_.quoteAsset)) {
        SPDLOG_INFO("Decided to buy {} for all available {}", assetSymbol, config_.quoteAsset);
        marketService_.makeMarketTypeOrderWithQuoteQuantity(tradingPairSymbol, ApiGateway::OrderSide::Buy,
                                                            ownedAssetsAmounts->at(config_.quoteAsset));
      }
    } else if (assetHistory.isSellSignalled()) {
      SPDLOG_INFO("Sell signal detected for asset {}", assetSymbol.val_);
      cancelOrdersAndInitOwnedAssetsAmounts();
      if (ownedAssetsAmounts->contains(assetSymbol)) {
        SPDLOG_INFO("Decided to sell all available {}", assetSymbol);
        marketService_.makeOrder(tradingPairSymbol, ApiGateway::OrderSide::Sell, ownedAssetsAmounts->at(assetSymbol),
                                 std::nullopt);
      }
    } else {
      SPDLOG_INFO("No buy/sell signal detected for {}", assetSymbol);
    }
  }
}

}  // namespace BotAlgorithms::MovingAverageCrossover
