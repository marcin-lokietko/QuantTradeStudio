#include "BotBacktester/Evaluator/Evaluator.hpp"

#include "gmock/gmock.h"

namespace BotBacktester::Evaluator {

using testing::_;
using testing::Return;

using ApiGateway::AssetSymbol;
using ApiGateway::TradingPairSymbol;

const TradingPairSymbol btcUsdtTradingPair{AssetSymbol{"BTC"}, AssetSymbol{"USDT"}};
const TradingPairSymbol ethUsdtTradingPair{AssetSymbol{"ETH"}, AssetSymbol{"USDT"}};

const auto time0 = std::chrono::sys_days(std::chrono::year{2025} / std::chrono::January / 1) + std::chrono::hours(12);
const auto time1 = time0 + std::chrono::minutes(1);
const auto time2 = time1 + std::chrono::minutes(1);
const auto time3 = time2 + std::chrono::minutes(1);

const std::map<TradingPairSymbol, MarketService::KlineSequence> klines = {
    {btcUsdtTradingPair,
     {MarketService::Kline{
          .openTime = time0,
          .closeTime = time1,
          .openPrice = ApiGateway::Price{"80000"},
          .closePrice = ApiGateway::Price{""},
          .lowPrice = ApiGateway::Price{""},
          .highPrice = ApiGateway::Price{""},
      },
      MarketService::Kline{
          .openTime = time1,
          .closeTime = time2,
          .openPrice = ApiGateway::Price{"90000"},
          .closePrice = ApiGateway::Price{""},
          .lowPrice = ApiGateway::Price{""},
          .highPrice = ApiGateway::Price{""},
      },
      MarketService::Kline{
          .openTime = time2,
          .closeTime = time3,
          .openPrice = ApiGateway::Price{"100000"},
          .closePrice = ApiGateway::Price{""},
          .lowPrice = ApiGateway::Price{""},
          .highPrice = ApiGateway::Price{""},
      }}},
    {ethUsdtTradingPair,
     {MarketService::Kline{
          .openTime = time0,
          .closeTime = time1,
          .openPrice = ApiGateway::Price{"2000"},
          .closePrice = ApiGateway::Price{""},
          .lowPrice = ApiGateway::Price{""},
          .highPrice = ApiGateway::Price{""},
      },
      MarketService::Kline{
          .openTime = time1,
          .closeTime = time2,
          .openPrice = ApiGateway::Price{"3000"},
          .closePrice = ApiGateway::Price{""},
          .lowPrice = ApiGateway::Price{""},
          .highPrice = ApiGateway::Price{""},
      },
      MarketService::Kline{
          .openTime = time2,
          .closeTime = time3,
          .openPrice = ApiGateway::Price{"4000"},
          .closePrice = ApiGateway::Price{""},
          .lowPrice = ApiGateway::Price{""},
          .highPrice = ApiGateway::Price{""},
      }}},
};

const ApiGateway::AssetQuantities initialOwnedAssets = {
    {AssetSymbol{"USDT"}, ApiGateway::AssetQuantity{"100000"}, ApiGateway::MarketId::Unknown},
    {AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"1"}, ApiGateway::MarketId::Unknown},
    {AssetSymbol{"ETH"}, ApiGateway::AssetQuantity{"10"}, ApiGateway::MarketId::Unknown},
};

const ApiGateway::AssetQuantitiesAndValuesHistory expectedAssetHistoryIfHeld{
    {time0,
     {
         {AssetSymbol{"USDT"}, ApiGateway::AssetQuantity{"100000"}, ApiGateway::Value{"100000"}},
         {AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"1"}, ApiGateway::Value{"80000.000000"}},
         {AssetSymbol{"ETH"}, ApiGateway::AssetQuantity{"10"}, ApiGateway::Value{"20000.000000"}},
     }},
    {time1,
     {
         {AssetSymbol{"USDT"}, ApiGateway::AssetQuantity{"100000"}, ApiGateway::Value{"100000"}},
         {AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"1"}, ApiGateway::Value{"90000.000000"}},
         {AssetSymbol{"ETH"}, ApiGateway::AssetQuantity{"10"}, ApiGateway::Value{"30000.000000"}},
     }},
    {time2,
     {
         {AssetSymbol{"USDT"}, ApiGateway::AssetQuantity{"100000"}, ApiGateway::Value{"100000"}},
         {AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"1"}, ApiGateway::Value{"100000.000000"}},
         {AssetSymbol{"ETH"}, ApiGateway::AssetQuantity{"10"}, ApiGateway::Value{"40000.000000"}},
     }},
};

class EvaluatorTest : public ::testing::Test {
 public:
  Evaluator sut_{klines};
};

TEST_F(EvaluatorTest, WhenSingleElementHistory_ThenResultsIfTradedEqualToResultsIfHeld) {
  const BotAssetsHistory history = {{time0, initialOwnedAssets}};
  const auto results = sut_.evaluate(history);

  EXPECT_EQ(expectedAssetHistoryIfHeld, results.botAssetHistory);
  EXPECT_EQ(ApiGateway::AssetQuantity{"40000.000000"}, results.totalProfitOrLossInAbsolute);
  EXPECT_DOUBLE_EQ(20., results.totalProfitOrLossInPercent->val_);  // 40000 / 200000 = 20 percent

  EXPECT_EQ(results.botAssetHistory, results.assetHistoryIfHeld);
  EXPECT_EQ(results.totalProfitOrLossInAbsolute, results.totalProfitOrLossInAbsoluteIfHeld);
  EXPECT_EQ(results.totalProfitOrLossInPercent, results.totalProfitOrLossInPercentIfHeld);

  EXPECT_EQ(ApiGateway::AssetSymbol{"USDT"}, results.absoluteAsset.value());
}

TEST_F(EvaluatorTest, WhenHistoryWithChangingAmounts_ThenResultsCalculatedBasedOnKlinesAndOwnedAmounts) {
  const BotAssetsHistory history = {
      {time0, initialOwnedAssets},
      {time1,
       {
           {AssetSymbol{"USDT"}, ApiGateway::AssetQuantity{"95000"}, ApiGateway::MarketId::Unknown},
           {AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"0.5"}, ApiGateway::MarketId::Unknown},
           {AssetSymbol{"ETH"}, ApiGateway::AssetQuantity{"30"}, ApiGateway::MarketId::Unknown},
       }},
  };
  const auto results = sut_.evaluate(history);

  const ApiGateway::AssetQuantitiesAndValuesHistory expectedBotAssetHistory{
      {time0,
       {
           {AssetSymbol{"USDT"}, ApiGateway::AssetQuantity{"100000"}, ApiGateway::Value{"100000"}},
           {AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"1"}, ApiGateway::Value{"80000.000000"}},
           {AssetSymbol{"ETH"}, ApiGateway::AssetQuantity{"10"}, ApiGateway::Value{"20000.000000"}},
       }},
      // Orders are made at time1, but at this exact moment values are still based on assets amounts before the trade is
      // finished:
      {time1,
       {
           {AssetSymbol{"USDT"}, ApiGateway::AssetQuantity{"100000"}, ApiGateway::Value{"100000"}},
           {AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"1"}, ApiGateway::Value{"90000.000000"}},
           {AssetSymbol{"ETH"}, ApiGateway::AssetQuantity{"10"}, ApiGateway::Value{"30000.000000"}},
       }},
      {time2,
       {
           {AssetSymbol{"USDT"}, ApiGateway::AssetQuantity{"95000"}, ApiGateway::Value{"95000"}},
           {AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"0.5"}, ApiGateway::Value{"50000.000000"}},
           {AssetSymbol{"ETH"}, ApiGateway::AssetQuantity{"30"}, ApiGateway::Value{"120000.000000"}},
       }},
  };

  EXPECT_EQ(expectedBotAssetHistory, results.botAssetHistory);
  EXPECT_EQ(ApiGateway::AssetQuantity{"65000.000000"}, results.totalProfitOrLossInAbsolute);
  EXPECT_DOUBLE_EQ(32.5, results.totalProfitOrLossInPercent.value().val_);  // 65000 / 200000 = 32.5 percent

  EXPECT_EQ(expectedAssetHistoryIfHeld, results.assetHistoryIfHeld);
  EXPECT_EQ(ApiGateway::AssetQuantity{"40000.000000"}, results.totalProfitOrLossInAbsoluteIfHeld);
  EXPECT_DOUBLE_EQ(20., results.totalProfitOrLossInPercentIfHeld->val_);

  EXPECT_EQ(ApiGateway::AssetSymbol{"USDT"}, results.absoluteAsset.value());
}

}  // namespace BotBacktester::Evaluator
