#pragma once

#include "ApiGateway/BacktestResults.hpp"
#include "BotBacktester/BotAssetsHistory.hpp"
#include "MarketService/KlineSequence.hpp"
#include "spdlog/spdlog.h"

namespace BotBacktester::Evaluator {

class Evaluator {
 public:
  Evaluator(std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> klines)
      : klines_(std::move(klines)) {}

  ApiGateway::BacktestResults evaluate(const BotAssetsHistory& botAssetsHistory) {
    ApiGateway::BacktestResults results;
    results.botAssetHistory = getExtendedBotAssetHistory(botAssetsHistory);

    results.totalProfitOrLossInAbsolute = getAbsoluteTotalProfitOrLoss(results.botAssetHistory.value());
    results.totalProfitOrLossInPercent =
        getPercentTotalProfitOrLoss(results.botAssetHistory.value(), results.totalProfitOrLossInAbsolute.value());
    // results.winRate =
    // results.averageWinSizeInAbsolute =
    // results.averageLossSizeInAbsolute =

    const BotAssetsHistory initialStateHistory{{botAssetsHistory[0].first, botAssetsHistory[0].second}};
    results.assetHistoryIfHeld = getExtendedBotAssetHistory(initialStateHistory);

    results.totalProfitOrLossInAbsoluteIfHeld = getAbsoluteTotalProfitOrLoss(results.assetHistoryIfHeld.value());
    results.totalProfitOrLossInPercentIfHeld = getPercentTotalProfitOrLoss(
        results.assetHistoryIfHeld.value(), results.totalProfitOrLossInAbsoluteIfHeld.value());

    results.absoluteAsset = ApiGateway::AssetSymbol{"USDT"};

    return results;
  }

 private:
  ApiGateway::AssetQuantitiesAndValuesHistory getExtendedBotAssetHistory(const BotAssetsHistory& botAssetsHistory) {
    ApiGateway::AssetQuantitiesAndValuesHistory extendedHistory;

    // BotAssetsHistory should always start with a state before the bot is executed
    pushAssetsSymbolsAndQuantities(extendedHistory, botAssetsHistory[0].first, botAssetsHistory[0].second);
    size_t nextAssetChangeIdx = 1;

    // Skip to the first kline that starts after botAssetsHistory[0].first
    size_t klineStartIdx = 0;
    for (const auto& kline : klines_.begin()->second) {
      if (kline.openTime >= botAssetsHistory[0].first) {
        break;
      }
      ++klineStartIdx;
    }
    fillAssetsValues(extendedHistory.back(), klineStartIdx);

    // Assumed that every kline sequence has the same open and close time
    // Limitation - only one trade can be simulated per single kline
    for (size_t klineIdx = klineStartIdx + 1; klineIdx < klines_.begin()->second.size(); ++klineIdx) {
      const auto& klineOpenTime = klines_.begin()->second[klineIdx].openTime;
      const bool isAssetChangeDoneBeforeKline =
          nextAssetChangeIdx < botAssetsHistory.size() && botAssetsHistory[nextAssetChangeIdx].first < klineOpenTime;
      if (isAssetChangeDoneBeforeKline) {
        pushAssetsSymbolsAndQuantities(extendedHistory, klineOpenTime, botAssetsHistory[nextAssetChangeIdx].second);
        ++nextAssetChangeIdx;
      } else {
        pushAssetsSymbolsAndQuantitiesFromPrevious(extendedHistory, klineOpenTime);
      }

      fillAssetsValues(extendedHistory.back(), klineIdx);
    }

    return extendedHistory;
  }

  void pushAssetsSymbolsAndQuantities(ApiGateway::AssetQuantitiesAndValuesHistory& extendedHistory,
                                      const std::chrono::system_clock::time_point& timePoint,
                                      const ApiGateway::AssetQuantities& quantities) {
    ApiGateway::AssetValues assetValues;
    for (const auto& singleAssetQuantity : quantities) {
      ApiGateway::SingleAssetValue singleAssetValue{.assetSymbol = singleAssetQuantity.assetSymbol,
                                                    .freeQuantity = singleAssetQuantity.freeQuantity,
                                                    .usdtValue = ApiGateway::Value{"0"}};
      assetValues.emplace_back(singleAssetValue);
    }

    extendedHistory.emplace_back(timePoint, assetValues);
  }

  void pushAssetsSymbolsAndQuantitiesFromPrevious(ApiGateway::AssetQuantitiesAndValuesHistory& extendedHistory,
                                                  const std::chrono::system_clock::time_point& timePoint) {
    if (extendedHistory.empty()) return;

    const auto& previous = extendedHistory.back();
    ApiGateway::AssetValues newValues;

    for (const auto& singleAssetValue : previous.second) {
      ApiGateway::SingleAssetValue newValue{.assetSymbol = singleAssetValue.assetSymbol,
                                            .freeQuantity = singleAssetValue.freeQuantity,
                                            .usdtValue = ApiGateway::Value{"0"}};
      newValues.emplace_back(newValue);
    }

    extendedHistory.emplace_back(timePoint, newValues);
  }

  void fillAssetsValues(ApiGateway::AssetQuantitiesAndValuesHistory::value_type& historyEntry, size_t klineIndex) {
    for (auto& singleAssetValue : historyEntry.second) {
      const auto& tradingPairSymbol =
          ApiGateway::TradingPairSymbol{singleAssetValue.assetSymbol, ApiGateway::AssetSymbol{"USDT"}};
      const auto it = klines_.find(tradingPairSymbol);
      if (it != klines_.end() && klineIndex < it->second.size()) {
        const auto& kline = it->second[klineIndex];
        singleAssetValue.usdtValue = ApiGateway::Value{
            std::to_string(std::stod(kline.openPrice.val_) * std::stod(singleAssetValue.freeQuantity.val_))};
      } else {
        SPDLOG_WARN("No price data available for trading pair: {}", toString(tradingPairSymbol));
      }
    }
  }

  ApiGateway::AssetQuantity getAbsoluteTotalProfitOrLoss(
      const ApiGateway::AssetQuantitiesAndValuesHistory& history) const {
    double initialTotalValue = getTotalValue(history.front().second);
    double endTotalValue = getTotalValue(history.back().second);

    return ApiGateway::AssetQuantity{std::to_string(endTotalValue - initialTotalValue)};
  }

  double getTotalValue(const ApiGateway::AssetValues& assetValues) const {
    double totalValue = 0.;
    for (const auto& singleAssetValue : assetValues) {
      totalValue += std::stod(singleAssetValue.usdtValue.val_);
    }
    return totalValue;
  }

  ApiGateway::Percent getPercentTotalProfitOrLoss(const ApiGateway::AssetQuantitiesAndValuesHistory& history,
                                                  const ApiGateway::AssetQuantity& totalProfitOrLoss) const {
    double initialTotalValue = getTotalValue(history.front().second);
    double totalProfitOrLossDouble = std::stod(totalProfitOrLoss.val_);

    double percent = (totalProfitOrLossDouble / initialTotalValue) * 100;
    return ApiGateway::Percent{percent};
  }

  std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> klines_;
};

}  // namespace BotBacktester::Evaluator