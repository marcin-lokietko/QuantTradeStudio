#include "BotAlgorithms/DonchianChannelBreakoutStrategy/DonchianChannelBreakoutStrategy.hpp"

#include <ranges>

#include "ApiGateway/KlineInterval.hpp"
#include "GeneratedMocks/HistoricalMarketDataProviderMock.hpp"
#include "GeneratedMocks/MarketServiceMock.hpp"
#include "GeneratedMocks/TimeMock.hpp"
#include "gmock/gmock.h"

namespace BotAlgorithms::DonchianChannelBreakoutStrategy {

using ApiGateway::AssetSymbol;
using ApiGateway::ExecutionPeriod;
using ApiGateway::IsExecutedImmediately;
using ApiGateway::SharePercent;
using ApiGateway::SingleAssetShare;
using ApiGateway::TradingPairSymbol;
using testing::_;
using testing::Return;

const auto currentTime =
    std::chrono::sys_days(std::chrono::year{2025} / std::chrono::January / 1) + std::chrono::hours(12);

const auto currentTimeAsTimePoint = std::chrono::time_point_cast<std::chrono::system_clock::duration>(currentTime);

const auto currentTimeAsTimeT = std::chrono::system_clock::to_time_t(currentTime);

const TradingPairSymbol btcUsdtTradingPair{AssetSymbol{"BTC"}, AssetSymbol{"USDT"}};
const TradingPairSymbol ethUsdtTradingPair{AssetSymbol{"ETH"}, AssetSymbol{"USDT"}};

const Config config{.executionInterval = ApiGateway::KlineInterval::OneHour,
                    .entryChannelLength = ApiGateway::AssetHistoryLength{10},
                    .exitChannelLength = ApiGateway::AssetHistoryLength{5},
                    .quoteAsset = AssetSymbol{"USDT"},
                    .baseAssets = {AssetSymbol{"BTC"}, AssetSymbol{"ETH"}}};

const ApiGateway::SingleAssetQuantity ownedUsdt{AssetSymbol{"USDT"}, ApiGateway::AssetQuantity{"1000"}};
const ApiGateway::SingleAssetQuantity ownedBtc{AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"1"}};
const ApiGateway::AssetQuantities ownedAssetQuantities{
    ownedUsdt, ownedBtc, ApiGateway::SingleAssetQuantity{AssetSymbol{"ETH"}, ApiGateway::AssetQuantity{"10"}}};

const MarketService::KlineSequence emptyKlinesSequence{};

const MarketService::Kline kline{.lowPrice = ApiGateway::Price{"9900"}, .highPrice = ApiGateway::Price{"10100"}};

const MarketService::KlineSequence tenElementsKlinesSequence{
    kline,
    kline,
    kline,
    {.lowPrice = ApiGateway::Price{"9000"},
     .highPrice = ApiGateway::Price{"11000"}},  // note this is outside exit channel, but inside entry channel
    kline,
    kline,
    kline,
    kline,
    {.lowPrice = ApiGateway::Price{"9500"}, .highPrice = ApiGateway::Price{"10100"}},
    kline};

class DonchianChannelBreakoutStrategyTest : public ::testing::Test {
 public:
  DonchianChannelBreakoutStrategyTest()
      : klinesStartTimePoint_(currentTimeAsTimePoint -
                              std::chrono::duration_cast<std::chrono::system_clock::duration>(
                                  toMilliseconds(config.executionInterval) * config.entryChannelLength.val_)) {
    EXPECT_CALL(timeMock_, getTimeSinceEpoch()).WillRepeatedly(Return(currentTimeAsTimeT));
  }

  void expectSleep(std::chrono::milliseconds expectedDelay, int expectedNumCalls) {
    auto numCallsSoFar = std::make_shared<int>(0);

    EXPECT_CALL(timeMock_, sleepFor(_, expectedDelay))
        .Times(expectedNumCalls)
        .WillRepeatedly([&source = source_, numCallsSoFar, expectedNumCalls]() {
          if (++(*numCallsSoFar) >= expectedNumCalls) {
            source.request_stop();
          }
        });
  }

  void mockEmptyKlinesHistory(const TradingPairSymbol& tradingPairSymbol) {
    EXPECT_CALL(historicalMarketDataProviderMock_,
                getKlines(tradingPairSymbol, config.executionInterval, klinesStartTimePoint_, currentTimeAsTimePoint))
        .WillOnce(Return(emptyKlinesSequence));
  }

  DonchianChannelBreakoutStrategy getSut(Config config) {
    return {std::move(config), marketServiceMock_, historicalMarketDataProviderMock_, timeMock_};
  }

  testing::StrictMock<MarketService::MarketServiceMock> marketServiceMock_;
  testing::StrictMock<MarketService::HistoricalMarketDataProviderMock> historicalMarketDataProviderMock_;
  testing::StrictMock<Time::TimeMock> timeMock_;

  std::stop_source source_;
  std::stop_token token_ = source_.get_token();

  const std::chrono::system_clock::time_point klinesStartTimePoint_;
};

TEST_F(DonchianChannelBreakoutStrategyTest, WhenKlinesCannotBeFetched_ThenBotDoesNotTrade) {
  expectSleep(toMilliseconds(config.executionInterval), 1);

  mockEmptyKlinesHistory(btcUsdtTradingPair);
  mockEmptyKlinesHistory(ethUsdtTradingPair);

  getSut(config).run(token_);
}

TEST_F(DonchianChannelBreakoutStrategyTest, WhenCurrentClosePriceDoesNotBreakoutFromChannels_ThenBotDoesNotTrade) {
  expectSleep(toMilliseconds(config.executionInterval), 1);

  auto btcUsdtKlines = tenElementsKlinesSequence;
  btcUsdtKlines.push_back({.closePrice = ApiGateway::Price{"10000"}});

  EXPECT_CALL(historicalMarketDataProviderMock_,
              getKlines(btcUsdtTradingPair, config.executionInterval, klinesStartTimePoint_, currentTimeAsTimePoint))
      .WillOnce(Return(btcUsdtKlines));

  mockEmptyKlinesHistory(ethUsdtTradingPair);

  getSut(config).run(token_);
}

TEST_F(DonchianChannelBreakoutStrategyTest, WhenCurrentClosePriceIsLowerThanExitChannelMinimum_ThenBotSells) {
  expectSleep(toMilliseconds(config.executionInterval), 1);

  auto btcUsdtKlines = tenElementsKlinesSequence;
  btcUsdtKlines.push_back({.closePrice = ApiGateway::Price{"9400"}});

  EXPECT_CALL(historicalMarketDataProviderMock_,
              getKlines(btcUsdtTradingPair, config.executionInterval, klinesStartTimePoint_, currentTimeAsTimePoint))
      .WillOnce(Return(btcUsdtKlines));

  mockEmptyKlinesHistory(ethUsdtTradingPair);

  ApiGateway::Orders openOrders{
      ApiGateway::SingleOrder{btcUsdtTradingPair, ApiGateway::OrderId{1}, ApiGateway::Price{"10000"},
                              ApiGateway::AssetQuantity{"0.1"}, ApiGateway::AssetQuantity{"0.05"},
                              ApiGateway::OrderSide::Buy},
      ApiGateway::SingleOrder{ethUsdtTradingPair, ApiGateway::OrderId{2}, ApiGateway::Price{"2000"},
                              ApiGateway::AssetQuantity{"0.5"}, ApiGateway::AssetQuantity{"0.25"},
                              ApiGateway::OrderSide::Sell}};

  EXPECT_CALL(marketServiceMock_, getOpenOrders()).WillOnce(Return(openOrders));
  EXPECT_CALL(marketServiceMock_, cancelAllOrdersOnASymbol(btcUsdtTradingPair))
      .WillOnce(Return(ApiGateway::OrderResult::Success));
  EXPECT_CALL(marketServiceMock_, cancelAllOrdersOnASymbol(ethUsdtTradingPair))
      .WillOnce(Return(ApiGateway::OrderResult::Success));

  EXPECT_CALL(marketServiceMock_, getOwnedAssetsQuantity()).WillOnce(Return(ownedAssetQuantities));
  EXPECT_CALL(marketServiceMock_, makeOrder(btcUsdtTradingPair, ApiGateway::OrderSide::Sell, ownedBtc.freeQuantity,
                                            testing::Eq(std::nullopt)))
      .WillOnce(Return(ApiGateway::OrderResult::Success));

  getSut(config).run(token_);
}

TEST_F(DonchianChannelBreakoutStrategyTest, WhenCurrentClosePriceIsHigherThanEntryChannelMaximum_ThenBotBuys) {
  expectSleep(toMilliseconds(config.executionInterval), 1);

  auto ethUsdtKlines = tenElementsKlinesSequence;
  ethUsdtKlines.push_back({.closePrice = ApiGateway::Price{"12000"}});

  EXPECT_CALL(historicalMarketDataProviderMock_,
              getKlines(ethUsdtTradingPair, config.executionInterval, klinesStartTimePoint_, currentTimeAsTimePoint))
      .WillOnce(Return(ethUsdtKlines));

  mockEmptyKlinesHistory(btcUsdtTradingPair);

  ApiGateway::Orders openOrders{
      ApiGateway::SingleOrder{btcUsdtTradingPair, ApiGateway::OrderId{1}, ApiGateway::Price{"10000"},
                              ApiGateway::AssetQuantity{"0.1"}, ApiGateway::AssetQuantity{"0.05"},
                              ApiGateway::OrderSide::Buy},
      ApiGateway::SingleOrder{ethUsdtTradingPair, ApiGateway::OrderId{2}, ApiGateway::Price{"2000"},
                              ApiGateway::AssetQuantity{"0.5"}, ApiGateway::AssetQuantity{"0.25"},
                              ApiGateway::OrderSide::Sell}};

  EXPECT_CALL(marketServiceMock_, getOpenOrders()).WillOnce(Return(openOrders));
  EXPECT_CALL(marketServiceMock_, cancelAllOrdersOnASymbol(btcUsdtTradingPair))
      .WillOnce(Return(ApiGateway::OrderResult::Success));
  EXPECT_CALL(marketServiceMock_, cancelAllOrdersOnASymbol(ethUsdtTradingPair))
      .WillOnce(Return(ApiGateway::OrderResult::Success));

  EXPECT_CALL(marketServiceMock_, getOwnedAssetsQuantity()).WillOnce(Return(ownedAssetQuantities));
  EXPECT_CALL(marketServiceMock_, makeMarketTypeOrderWithQuoteQuantity(ethUsdtTradingPair, ApiGateway::OrderSide::Buy,
                                                                       ownedUsdt.freeQuantity))
      .WillOnce(Return(ApiGateway::OrderResult::Success));

  getSut(config).run(token_);
}

}  // namespace BotAlgorithms::DonchianChannelBreakoutStrategy
