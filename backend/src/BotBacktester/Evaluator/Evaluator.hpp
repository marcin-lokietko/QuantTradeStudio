#pragma once

#include <map>

#include "ApiGateway/BacktestResults.hpp"
#include "ApiGateway/TradingPairSymbol.hpp"
#include "BotBacktester/BotAssetsHistory.hpp"
#include "MarketService/KlineSequence.hpp"

namespace BotBacktester::Evaluator {

class Evaluator {
 public:
  Evaluator(std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> klines);

  ApiGateway::BacktestResults evaluate(const BotAssetsHistory& botAssetsHistory);

 private:
  ApiGateway::AssetQuantitiesAndValuesHistory getExtendedBotAssetHistory(const BotAssetsHistory& botAssetsHistory);

  void pushAssetsSymbolsAndQuantities(ApiGateway::AssetQuantitiesAndValuesHistory& extendedHistory,
                                      const std::chrono::system_clock::time_point& timePoint,
                                      const ApiGateway::AssetQuantities& quantities);

  void pushAssetsSymbolsAndQuantitiesFromPrevious(ApiGateway::AssetQuantitiesAndValuesHistory& extendedHistory,
                                                  const std::chrono::system_clock::time_point& timePoint);

  void fillAssetsValues(ApiGateway::AssetQuantitiesAndValuesHistory::value_type& historyEntry, size_t klineIndex);

  ApiGateway::AssetQuantity getAbsoluteTotalProfitOrLoss(
      const ApiGateway::AssetQuantitiesAndValuesHistory& history) const;

  double getTotalValue(const ApiGateway::AssetValues& assetValues) const;

  ApiGateway::Percent getPercentTotalProfitOrLoss(const ApiGateway::AssetQuantitiesAndValuesHistory& history,
                                                  const ApiGateway::AssetQuantity& totalProfitOrLoss) const;

  std::map<ApiGateway::TradingPairSymbol, MarketService::KlineSequence> klines_;
};

}  // namespace BotBacktester::Evaluator