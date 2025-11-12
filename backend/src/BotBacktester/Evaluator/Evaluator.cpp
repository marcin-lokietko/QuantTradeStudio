#include "BotBacktester/Evaluator/Evaluator.hpp"

#include <spdlog/spdlog.h>

#include "BotBacktester/Utils.hpp"

namespace BotBacktester::Evaluator {

Evaluator::Evaluator(std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> klines)
    : klines_(std::move(klines)) {
  if (doKlinesHaveDifferentSizesOrTimeRanges(klines_)) {
    throw EvaluatorException("Contract broken - klines provided to Evaluator have different sizes or time ranges");
  }
}

ApiGateway::BacktestResults Evaluator::evaluate(const BotAssetsHistory& botAssetsHistory) {
  ApiGateway::BacktestResults results;
  results.botAssetHistory = calcBotAssetHistoryWithValues(botAssetsHistory);

  results.totalProfitOrLossInAbsolute = getAbsoluteTotalProfitOrLoss(results.botAssetHistory.value());
  results.totalProfitOrLossInPercent =
      getPercentTotalProfitOrLoss(results.botAssetHistory.value(), results.totalProfitOrLossInAbsolute.value());
  // results.winRate =
  // results.averageWinSizeInAbsolute =
  // results.averageLossSizeInAbsolute =

  const BotAssetsHistory initialStateHistory{{botAssetsHistory[0].first, botAssetsHistory[0].second}};
  results.assetHistoryIfHeld = calcBotAssetHistoryWithValues(initialStateHistory);

  results.totalProfitOrLossInAbsoluteIfHeld = getAbsoluteTotalProfitOrLoss(results.assetHistoryIfHeld.value());
  results.totalProfitOrLossInPercentIfHeld = getPercentTotalProfitOrLoss(
      results.assetHistoryIfHeld.value(), results.totalProfitOrLossInAbsoluteIfHeld.value());

  results.absoluteAsset = ApiGateway::AssetSymbol{"USDT"};

  return results;
}

ApiGateway::AssetQuantitiesAndValuesHistory Evaluator::calcBotAssetHistoryWithValues(
    const BotAssetsHistory& botAssetsHistory) {
  ApiGateway::AssetQuantitiesAndValuesHistory extendedHistory;

  // BotAssetsHistory should always start with a state before the bot is executed
  pushAssetsSymbolsAndQuantities(extendedHistory, botAssetsHistory[0].first, botAssetsHistory[0].second);

  // Skip to the first kline that starts after botAssetsHistory[0].first and use it to fill values
  size_t klineStartIdx = getIdxOfFirstKlineAtOrAfterTimePoint(botAssetsHistory[0].first);
  fillAssetsValues(extendedHistory.back(), klineStartIdx);

  size_t nextAssetChangeIdx = 1;
  const auto& firstKlineSequence = klines_.begin()->second;

  for (size_t klineIdx = klineStartIdx + 1; klineIdx < firstKlineSequence.size(); ++klineIdx) {
    const auto& klineOpenTime = firstKlineSequence[klineIdx].openTime;

    auto isAssetChangeDoneBeforeCurrentKline = [&botAssetsHistory, &klineOpenTime](const size_t nextAssetChangeIdx) {
      return nextAssetChangeIdx < botAssetsHistory.size() && botAssetsHistory[nextAssetChangeIdx].first < klineOpenTime;
    };
    if (isAssetChangeDoneBeforeCurrentKline(nextAssetChangeIdx)) {
      // Multiple trades can happen between two klines, so we need to find the last one before the kline open time and
      // save it's state as nest AssetQuantitiesAndValuesHistory entry
      while (isAssetChangeDoneBeforeCurrentKline(nextAssetChangeIdx + 1)) {
        ++nextAssetChangeIdx;
      }
      pushAssetsSymbolsAndQuantities(extendedHistory, klineOpenTime, botAssetsHistory[nextAssetChangeIdx].second);

      // next time start with the next asset change
      ++nextAssetChangeIdx;
    } else {
      pushAssetsSymbolsAndQuantitiesFromPreviousHistoryEntry(extendedHistory, klineOpenTime);
    }

    fillAssetsValues(extendedHistory.back(), klineIdx);
  }

  return extendedHistory;
}

void Evaluator::pushAssetsSymbolsAndQuantities(ApiGateway::AssetQuantitiesAndValuesHistory& extendedHistory,
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

void Evaluator::pushAssetsSymbolsAndQuantitiesFromPreviousHistoryEntry(
    ApiGateway::AssetQuantitiesAndValuesHistory& extendedHistory,
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

size_t Evaluator::getIdxOfFirstKlineAtOrAfterTimePoint(const std::chrono::system_clock::time_point& timePoint) {
  size_t klineStartIdx = 0;
  for (const auto& kline : klines_.begin()->second) {
    if (kline.openTime >= timePoint) {
      break;
    }
    ++klineStartIdx;
  }
  return klineStartIdx;
}

void Evaluator::fillAssetsValues(ApiGateway::AssetQuantitiesAndValuesHistory::value_type& historyEntry,
                                 size_t klineIndex) {
  for (auto& singleAssetValue : historyEntry.second) {
    if (singleAssetValue.assetSymbol == ApiGateway::AssetSymbol{"USDT"}) {
      singleAssetValue.usdtValue = ApiGateway::Value{singleAssetValue.freeQuantity.val_};
      continue;
    }
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

ApiGateway::AssetQuantity Evaluator::getAbsoluteTotalProfitOrLoss(
    const ApiGateway::AssetQuantitiesAndValuesHistory& history) const {
  double initialTotalValue = getTotalValue(history.front().second);
  double endTotalValue = getTotalValue(history.back().second);

  return ApiGateway::AssetQuantity{std::to_string(endTotalValue - initialTotalValue)};
}

double Evaluator::getTotalValue(const ApiGateway::AssetValues& assetValues) const {
  double totalValue = 0.;
  for (const auto& singleAssetValue : assetValues) {
    totalValue += std::stod(singleAssetValue.usdtValue.val_);
  }
  return totalValue;
}

ApiGateway::Percent Evaluator::getPercentTotalProfitOrLoss(const ApiGateway::AssetQuantitiesAndValuesHistory& history,
                                                           const ApiGateway::AssetQuantity& totalProfitOrLoss) const {
  double initialTotalValue = getTotalValue(history.front().second);
  double totalProfitOrLossDouble = std::stod(totalProfitOrLoss.val_);

  double percent = (totalProfitOrLossDouble / initialTotalValue) * 100;
  return ApiGateway::Percent{percent};
}

}  // namespace BotBacktester::Evaluator