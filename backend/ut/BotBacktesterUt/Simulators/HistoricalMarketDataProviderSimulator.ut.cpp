#include "BotBacktester/Simulators/HistoricalMarketDataProviderSimulator.hpp"

#include "gmock/gmock.h"

namespace BotBacktester::Simulators {

using testing::_;
using testing::Return;

using ApiGateway::AssetSymbol;
using ApiGateway::TradingPairSymbol;

const TradingPairSymbol btcUsdtTradingPair{AssetSymbol{"BTC"}, AssetSymbol{"USDT"}};
const TradingPairSymbol ethUsdtTradingPair{AssetSymbol{"ETH"}, AssetSymbol{"USDT"}};

const auto time0 = std::chrono::sys_days(std::chrono::year{2025} / std::chrono::January / 1) + std::chrono::hours(12);
const auto time1 = time0 + std::chrono::minutes(1);
const auto time2 = time0 + std::chrono::minutes(2);
const auto time3 = time0 + std::chrono::minutes(3);
const auto time4 = time0 + std::chrono::minutes(4);
const auto time5 = time0 + std::chrono::minutes(5);
const auto time6 = time0 + std::chrono::minutes(6);
const auto time7 = time0 + std::chrono::minutes(7);
const auto time8 = time0 + std::chrono::minutes(8);
const auto time9 = time0 + std::chrono::minutes(9);
const auto time10 = time0 + std::chrono::minutes(10);

const std::map<TradingPairSymbol, MarketService::KlineSequence> defaultSourceKlines = {
    {btcUsdtTradingPair,
     {MarketService::Kline{
          .openTime = time0,
          .closeTime = time1,
          .openPrice = ApiGateway::Price{"80000"},
          .closePrice = ApiGateway::Price{"89000"},
          .lowPrice = ApiGateway::Price{"70000"},
          .highPrice = ApiGateway::Price{"90000"},
      },
      MarketService::Kline{
          .openTime = time1,
          .closeTime = time2,
          .openPrice = ApiGateway::Price{"90000"},
          .closePrice = ApiGateway::Price{"99000"},
          .lowPrice = ApiGateway::Price{"90000"},
          .highPrice = ApiGateway::Price{"99000"},
      },
      MarketService::Kline{
          .openTime = time2,
          .closeTime = time3,
          .openPrice = ApiGateway::Price{"100000"},
          .closePrice = ApiGateway::Price{"109000"},
          .lowPrice = ApiGateway::Price{"100000"},
          .highPrice = ApiGateway::Price{"109000"},
      }}},
    {ethUsdtTradingPair,
     {MarketService::Kline{
          .openTime = time0,
          .closeTime = time1,
          .openPrice = ApiGateway::Price{"2000"},
          .closePrice = ApiGateway::Price{"2900"},
          .lowPrice = ApiGateway::Price{"2000"},
          .highPrice = ApiGateway::Price{"2900"},
      },
      MarketService::Kline{
          .openTime = time1,
          .closeTime = time2,
          .openPrice = ApiGateway::Price{"3000"},
          .closePrice = ApiGateway::Price{"3900"},
          .lowPrice = ApiGateway::Price{"3000"},
          .highPrice = ApiGateway::Price{"3900"},
      },
      MarketService::Kline{
          .openTime = time2,
          .closeTime = time3,
          .openPrice = ApiGateway::Price{"4000"},
          .closePrice = ApiGateway::Price{"4900"},
          .lowPrice = ApiGateway::Price{"4000"},
          .highPrice = ApiGateway::Price{"4900"},
      }}},
};

class HistoricalMarketDataProviderSimulatorTest : public ::testing::Test {};

/*
TEST_F(HistoricalMarketDataProviderSimulatorTest, WhenNoSourceKlinesAvailableForRequestedSymbol_ThenExceptionThrown) {
  const TradingPairSymbol someUnknownTradingPair{AssetSymbol{"ABC"}, AssetSymbol{"XYZ"}};

  EXPECT_THROW(HistoricalMarketDataProviderSimulator(defaultSourceKlines)
                   .getKlines(someUnknownTradingPair, ApiGateway::KlineInterval::OneMinute, time0, time3),
               HistoricalMarketDataProviderSimulatorException);
}

TEST_F(HistoricalMarketDataProviderSimulatorTest, WhenNoSourceKlinesAreEmpty_ThenExceptionThrown) {
  std::map<TradingPairSymbol, MarketService::KlineSequence> klines{};

  klines[btcUsdtTradingPair] = {};

  EXPECT_THROW(HistoricalMarketDataProviderSimulator(klines).getKlines(
                   btcUsdtTradingPair, ApiGateway::KlineInterval::OneMinute, time0, time3),
               HistoricalMarketDataProviderSimulatorException);
}

TEST_F(HistoricalMarketDataProviderSimulatorTest, WhenRequestedKlinesStartBeforeSourceKlines_ThenExceptionThrown) {
  const auto requestedStartTime = time0 - std::chrono::minutes(1);

  EXPECT_THROW(HistoricalMarketDataProviderSimulator(defaultSourceKlines)
                   .getKlines(btcUsdtTradingPair, ApiGateway::KlineInterval::OneMinute, requestedStartTime, time3),
               HistoricalMarketDataProviderSimulatorException);
}

TEST_F(HistoricalMarketDataProviderSimulatorTest, WhenRequestedKlinesEndAfterSourceKlines_ThenExceptionThrown) {
  const auto requestedEndTime = time3 + std::chrono::minutes(1);

  EXPECT_THROW(HistoricalMarketDataProviderSimulator(defaultSourceKlines)
                   .getKlines(btcUsdtTradingPair, ApiGateway::KlineInterval::OneMinute, time0, requestedEndTime),
               HistoricalMarketDataProviderSimulatorException);
}

TEST_F(HistoricalMarketDataProviderSimulatorTest,
       WhenRequestedKlinesIntervalSmallerThanSourceKlinesInterval_ThenExceptionThrown) {
  const auto fiveMinTime0 = time0;
  const auto fiveMinTime1 = time0 + std::chrono::minutes(5);
  const auto fiveMinTime2 = time0 + std::chrono::minutes(10);

  std::map<TradingPairSymbol, MarketService::KlineSequence> fiveMinuteKlines = {
      {btcUsdtTradingPair,
       {MarketService::Kline{
            .openTime = fiveMinTime0,
            .closeTime = fiveMinTime1,
            .openPrice = ApiGateway::Price{"80000"},
            .closePrice = ApiGateway::Price{"85000"},
            .lowPrice = ApiGateway::Price{"79000"},
            .highPrice = ApiGateway::Price{"86000"},
        },
        MarketService::Kline{
            .openTime = fiveMinTime1,
            .closeTime = fiveMinTime2,
            .openPrice = ApiGateway::Price{"85000"},
            .closePrice = ApiGateway::Price{"90000"},
            .lowPrice = ApiGateway::Price{"84000"},
            .highPrice = ApiGateway::Price{"91000"},
        }}},
  };

  // Request 1-minute interval klines from 5-minute source data - should throw
  EXPECT_THROW(HistoricalMarketDataProviderSimulator(fiveMinuteKlines)
                   .getKlines(btcUsdtTradingPair, ApiGateway::KlineInterval::OneMinute, fiveMinTime0, fiveMinTime2),
               HistoricalMarketDataProviderSimulatorException);
}
*/
TEST_F(HistoricalMarketDataProviderSimulatorTest,
       GivenOneMinuteIntervalSource_WhenOneMinuteKlinesRequested_ThenValidKlinesReturned) {
  const auto result = HistoricalMarketDataProviderSimulator(defaultSourceKlines)
                          .getKlines(btcUsdtTradingPair, ApiGateway::KlineInterval::OneMinute, time0, time3);

  ASSERT_EQ(result.size(), 3);

  EXPECT_EQ(result[0].openTime, time0);
  EXPECT_EQ(result[0].closeTime, time1);
  EXPECT_EQ(result[0].openPrice.val_, "80000");
  EXPECT_EQ(result[0].closePrice.val_, "89000");
  EXPECT_EQ(result[0].lowPrice.val_, "70000.000000");
  EXPECT_EQ(result[0].highPrice.val_, "90000.000000");

  EXPECT_EQ(result[1].openTime, time1);
  EXPECT_EQ(result[1].closeTime, time2);
  EXPECT_EQ(result[1].openPrice.val_, "90000");
  EXPECT_EQ(result[1].closePrice.val_, "99000");
  EXPECT_EQ(result[1].lowPrice.val_, "90000.000000");
  EXPECT_EQ(result[1].highPrice.val_, "99000.000000");

  EXPECT_EQ(result[2].openTime, time2);
  EXPECT_EQ(result[2].closeTime, time3);
  EXPECT_EQ(result[2].openPrice.val_, "100000");
  EXPECT_EQ(result[2].closePrice.val_, "109000");
  EXPECT_EQ(result[2].lowPrice.val_, "100000.000000");
  EXPECT_EQ(result[2].highPrice.val_, "109000.000000");
}

TEST_F(HistoricalMarketDataProviderSimulatorTest,
       GivenOneMinuteIntervalSource_WhenOneMinuteKlinesRequestedWithNotAlignedBoundaries_ThenValidKlinesReturned) {
  const auto timeBegin = time0 + std::chrono::seconds(30);
  const auto timeEnd = time2 + std::chrono::seconds(30);

  const auto result = HistoricalMarketDataProviderSimulator(defaultSourceKlines)
                          .getKlines(btcUsdtTradingPair, ApiGateway::KlineInterval::OneMinute, timeBegin, timeEnd);

  ASSERT_EQ(result.size(), 2);

  // result[0] should be calculated based on first and second source klines
  EXPECT_EQ(result[0].openTime, timeBegin);
  EXPECT_EQ(result[0].closeTime, timeBegin + std::chrono::minutes(1));
  EXPECT_EQ(result[0].openPrice.val_, "80000");
  EXPECT_EQ(result[0].closePrice.val_, "99000");
  EXPECT_EQ(result[0].lowPrice.val_, "70000.000000");
  EXPECT_EQ(result[0].highPrice.val_, "99000.000000");

  // result[1] should be calculated based on second and third source klines
  EXPECT_EQ(result[1].openTime, timeBegin + std::chrono::minutes(1));
  EXPECT_EQ(result[1].closeTime, timeEnd);
  EXPECT_EQ(result[1].openPrice.val_, "90000");
  EXPECT_EQ(result[1].closePrice.val_, "109000");
  EXPECT_EQ(result[1].lowPrice.val_, "90000.000000");
  EXPECT_EQ(result[1].highPrice.val_, "109000.000000");
}

TEST_F(HistoricalMarketDataProviderSimulatorTest,
       GivenOneMinuteIntervalSource_WhenFiveMinutesKlinesRequested_ThenValidKlinesReturned) {
  const std::map<TradingPairSymbol, MarketService::KlineSequence> sourceKlines = {
      {btcUsdtTradingPair,
       {
           MarketService::Kline{
               .openTime = time0,
               .closeTime = time1,
               .openPrice = ApiGateway::Price{"10000"},
               .closePrice = ApiGateway::Price{"20000"},
               .lowPrice = ApiGateway::Price{"10000"},
               .highPrice = ApiGateway::Price{"20000"},
           },
           MarketService::Kline{
               .openTime = time1,
               .closeTime = time2,
               .openPrice = ApiGateway::Price{"20000"},
               .closePrice = ApiGateway::Price{"30000"},
               .lowPrice = ApiGateway::Price{"10"},
               .highPrice = ApiGateway::Price{"9999999"},
           },
           MarketService::Kline{
               .openTime = time2,
               .closeTime = time3,
               .openPrice = ApiGateway::Price{"30000"},
               .closePrice = ApiGateway::Price{"40000"},
               .lowPrice = ApiGateway::Price{"30000"},
               .highPrice = ApiGateway::Price{"40000"},
           },
           MarketService::Kline{
               .openTime = time3,
               .closeTime = time4,
               .openPrice = ApiGateway::Price{"40000"},
               .closePrice = ApiGateway::Price{"50000"},
               .lowPrice = ApiGateway::Price{"40000"},
               .highPrice = ApiGateway::Price{"50000"},
           },
           MarketService::Kline{
               .openTime = time4,
               .closeTime = time5,
               .openPrice = ApiGateway::Price{"50000"},
               .closePrice = ApiGateway::Price{"60000"},
               .lowPrice = ApiGateway::Price{"50000"},
               .highPrice = ApiGateway::Price{"60000"},
           },
           MarketService::Kline{
               .openTime = time5,
               .closeTime = time6,
               .openPrice = ApiGateway::Price{"60000"},
               .closePrice = ApiGateway::Price{"70000"},
               .lowPrice = ApiGateway::Price{"60000"},
               .highPrice = ApiGateway::Price{"70000"},
           },
           MarketService::Kline{
               .openTime = time6,
               .closeTime = time7,
               .openPrice = ApiGateway::Price{"70000"},
               .closePrice = ApiGateway::Price{"80000"},
               .lowPrice = ApiGateway::Price{"70000"},
               .highPrice = ApiGateway::Price{"80000"},
           },
           MarketService::Kline{
               .openTime = time7,
               .closeTime = time8,
               .openPrice = ApiGateway::Price{"80000"},
               .closePrice = ApiGateway::Price{"90000"},
               .lowPrice = ApiGateway::Price{"80000"},
               .highPrice = ApiGateway::Price{"90000"},
           },
           MarketService::Kline{
               .openTime = time8,
               .closeTime = time9,
               .openPrice = ApiGateway::Price{"90000"},
               .closePrice = ApiGateway::Price{"100000"},
               .lowPrice = ApiGateway::Price{"90000"},
               .highPrice = ApiGateway::Price{"100000"},
           },
           MarketService::Kline{
               .openTime = time9,
               .closeTime = time10,
               .openPrice = ApiGateway::Price{"100000"},
               .closePrice = ApiGateway::Price{"110000"},
               .lowPrice = ApiGateway::Price{"100000"},
               .highPrice = ApiGateway::Price{"110000"},
           },
       }}};

  const auto result = HistoricalMarketDataProviderSimulator(sourceKlines)
                          .getKlines(btcUsdtTradingPair, ApiGateway::KlineInterval::FiveMinutes, time0, time10);

  ASSERT_EQ(result.size(), 2);

  EXPECT_EQ(result[0].openTime, time0);
  EXPECT_EQ(result[0].closeTime, time5);
  EXPECT_EQ(result[0].openPrice.val_, "10000");
  EXPECT_EQ(result[0].closePrice.val_, "60000");
  EXPECT_EQ(result[0].lowPrice.val_, "10.000000");
  EXPECT_EQ(result[0].highPrice.val_, "9999999.000000");

  EXPECT_EQ(result[1].openTime, time5);
  EXPECT_EQ(result[1].closeTime, time10);
  EXPECT_EQ(result[1].openPrice.val_, "60000");
  EXPECT_EQ(result[1].closePrice.val_, "110000");
  EXPECT_EQ(result[1].lowPrice.val_, "60000.000000");
  EXPECT_EQ(result[1].highPrice.val_, "110000.000000");
}

}  // namespace BotBacktester::Simulators
