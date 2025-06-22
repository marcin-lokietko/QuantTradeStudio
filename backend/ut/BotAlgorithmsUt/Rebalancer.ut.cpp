#include "BotAlgorithms/Rebalancer/Rebalancer.hpp"
#include "GeneratedMocks/MarketServiceMock.hpp"
#include "GeneratedMocks/TimeMock.hpp"
#include "gmock/gmock.h"

namespace BotAlgorithms::Rebalancer {

using ApiGateway::AssetSymbol;
using ApiGateway::ExecutionPeriod;
using ApiGateway::IsExecutedImmediately;
using ApiGateway::SharePercent;
using ApiGateway::SingleAssetShare;
using ApiGateway::TradingPairSymbol;
using testing::_;
using testing::Return;

const TradingPairSymbol btcUsdtTradingPair{AssetSymbol{"BTC"}, AssetSymbol{"USDT"}};
const TradingPairSymbol ethUsdtTradingPair{AssetSymbol{"ETH"}, AssetSymbol{"USDT"}};
const TradingPairSymbol ethEurTradingPair{AssetSymbol{"ETH"}, AssetSymbol{"EUR"}};
const TradingPairSymbol ethPlnTradingPair{AssetSymbol{"ETH"}, AssetSymbol{"PLN"}};

const ApiGateway::AssetShares assetShares{SingleAssetShare{AssetSymbol{"BTC"}, SharePercent{80}},
                                          SingleAssetShare{AssetSymbol{"ETH"}, SharePercent{20}}};

const Config config{.executionPeriod = ExecutionPeriod{60},
                    .isExecutedImmediately = IsExecutedImmediately{false},
                    .quoteAsset = AssetSymbol{"USDT"},
                    .baseAssetShares = assetShares};

class RebalancerTest : public ::testing::Test {
 public:
  void expectSleep(std::chrono::seconds expectedDelay, uint64_t expectedNumCalls = 1) {
    EXPECT_CALL(timeMock_, sleepFor(_, std::chrono::duration_cast<std::chrono::milliseconds>(expectedDelay)))
        .Times(expectedNumCalls)
        .WillRepeatedly(Return());
  }

  void expectGetOpenOrdersCalls(ApiGateway::Orders mockedOrders, const int numCallsUntilRequestTopSimulated) {
    auto numCallsSoFar = std::make_shared<int>(0);

    EXPECT_CALL(marketServiceMock_, getOpenOrders())
        .WillRepeatedly([&source = source, mockedOrders, numCallsSoFar, numCallsUntilRequestTopSimulated]() mutable {
          if (++(*numCallsSoFar) >= numCallsUntilRequestTopSimulated) {
            source.request_stop();
          }
          return mockedOrders;
        });
  }

  Rebalancer getSut(Config config) { return {std::move(config), marketServiceMock_, timeMock_}; }

  testing::StrictMock<MarketService::MarketServiceMock> marketServiceMock_;
  testing::StrictMock<Time::TimeMock> timeMock_;

  std::stop_source source;
  std::stop_token token = source.get_token();
};

TEST_F(RebalancerTest, WhenConfiguredBaseAssetsNowOwned_ThenBotDoesNotTrade) {
  expectGetOpenOrdersCalls(ApiGateway::Orders{}, 1);

  EXPECT_CALL(marketServiceMock_, getOwnedAssetValues(ApiGateway::AssetSymbol{"USDT"}))
      .WillOnce(Return(MarketService::AssetValues{}));

  expectSleep(std::chrono::seconds(config.executionPeriod.val_));
  getSut(config).run(token);
}

TEST_F(RebalancerTest, WhenOrdersOnConfiguredTradingPairsAreOpen_ThenBotCancelsThemAndOnlyThem) {
  ApiGateway::Orders openOrders{
      ApiGateway::SingleOrder{btcUsdtTradingPair, ApiGateway::OrderId{1}, ApiGateway::Price{"10000"},
                              ApiGateway::AssetQuantity{"0.1"}, ApiGateway::AssetQuantity{"0.05"},
                              ApiGateway::OrderSide::Buy},
      ApiGateway::SingleOrder{ethUsdtTradingPair, ApiGateway::OrderId{2}, ApiGateway::Price{"2000"},
                              ApiGateway::AssetQuantity{"0.5"}, ApiGateway::AssetQuantity{"0.25"},
                              ApiGateway::OrderSide::Sell},
      ApiGateway::SingleOrder{ethPlnTradingPair, ApiGateway::OrderId{3}, ApiGateway::Price{"2000"},
                              ApiGateway::AssetQuantity{"0.5"}, ApiGateway::AssetQuantity{"0.25"},
                              ApiGateway::OrderSide::Sell},
      ApiGateway::SingleOrder{ethEurTradingPair, ApiGateway::OrderId{4}, ApiGateway::Price{"2000"},
                              ApiGateway::AssetQuantity{"0.5"}, ApiGateway::AssetQuantity{"0.25"},
                              ApiGateway::OrderSide::Sell}};
  expectGetOpenOrdersCalls(openOrders, 1);

  EXPECT_CALL(marketServiceMock_, cancelAllOrdersOnASymbol(btcUsdtTradingPair))
      .WillOnce(Return(ApiGateway::OrderResult::Success));
  EXPECT_CALL(marketServiceMock_, cancelAllOrdersOnASymbol(ethUsdtTradingPair))
      .WillOnce(Return(ApiGateway::OrderResult::Success));

  EXPECT_CALL(marketServiceMock_, getOwnedAssetValues(ApiGateway::AssetSymbol{"USDT"}))
      .WillOnce(Return(MarketService::AssetValues{}));

  expectSleep(std::chrono::seconds(config.executionPeriod.val_));
  getSut(config).run(token);
}

TEST_F(RebalancerTest, WhenConfiguredBaseAssetsHaveInvalidValues_ThenBotDoesNotTrade) {
  expectGetOpenOrdersCalls(ApiGateway::Orders{}, 1);

  EXPECT_CALL(marketServiceMock_, getOwnedAssetValues(ApiGateway::AssetSymbol{"USDT"}))
      .WillOnce(Return(MarketService::AssetValues{
          {MarketService::SingleAssetValue{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::AssetSymbol{"USDT"},
                                           ApiGateway::Value{"invalid_value"}},
           MarketService::SingleAssetValue{ApiGateway::AssetSymbol{"ETH"}, ApiGateway::AssetSymbol{"USDT"},
                                           ApiGateway::Value{"100.5"}}

          }}));

  expectSleep(std::chrono::seconds(config.executionPeriod.val_));
  getSut(config).run(token);
}

TEST_F(RebalancerTest, WhenOwnedAssetsSharedAreCloseToExpected_ThenBotDoesNotTrade) {
  expectGetOpenOrdersCalls(ApiGateway::Orders{}, 1);

  EXPECT_CALL(marketServiceMock_, getOwnedAssetValues(ApiGateway::AssetSymbol{"USDT"}))
      .WillOnce(Return(MarketService::AssetValues{
          {MarketService::SingleAssetValue{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::AssetSymbol{"USDT"},
                                           ApiGateway::Value{"80002.123"}},
           MarketService::SingleAssetValue{ApiGateway::AssetSymbol{"ETH"}, ApiGateway::AssetSymbol{"USDT"},
                                           ApiGateway::Value{"20001.321"}}}}));

  expectSleep(std::chrono::seconds(config.executionPeriod.val_));
  getSut(config).run(token);
}

TEST_F(RebalancerTest, WhenOwnedAssetsSharedDifferentThanExpected_ThenOrdersArePlaced) {
  expectGetOpenOrdersCalls(ApiGateway::Orders{}, 1);

  EXPECT_CALL(marketServiceMock_, getOwnedAssetValues(ApiGateway::AssetSymbol{"USDT"}))
      .WillOnce(Return(MarketService::AssetValues{
          {MarketService::SingleAssetValue{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::AssetSymbol{"USDT"},
                                           ApiGateway::Value{"500.0"}},
           MarketService::SingleAssetValue{ApiGateway::AssetSymbol{"ETH"}, ApiGateway::AssetSymbol{"USDT"},
                                           ApiGateway::Value{"500.0"}}}}));

  EXPECT_CALL(marketServiceMock_, makeMarketTypeOrderWithQuoteQuantity(btcUsdtTradingPair, _, _))
      .WillOnce([](const ApiGateway::TradingPairSymbol&, const ApiGateway::OrderSide& side,
                   const ApiGateway::AssetQuantity& quantity) {
        EXPECT_EQ(ApiGateway::OrderSide::Buy, side);
        EXPECT_NEAR(300.0, std::stod(quantity.val_), 0.001);
        return ApiGateway::OrderResult::Success;
      });

  EXPECT_CALL(marketServiceMock_, makeMarketTypeOrderWithQuoteQuantity(ethUsdtTradingPair, _, _))
      .WillOnce([](const ApiGateway::TradingPairSymbol&, const ApiGateway::OrderSide& side,
                   const ApiGateway::AssetQuantity& quantity) {
        EXPECT_EQ(ApiGateway::OrderSide::Sell, side);
        EXPECT_NEAR(300.0, std::stod(quantity.val_), 0.001);
        return ApiGateway::OrderResult::Success;
      });

  expectSleep(std::chrono::seconds(config.executionPeriod.val_));
  getSut(config).run(token);
}

TEST_F(RebalancerTest,
       WhenOwnedAssetsSharedDifferentThanExpectedAndBotStoppedAfter3Executions_ThenOrdersArePlaced3Times) {
  expectGetOpenOrdersCalls(ApiGateway::Orders{}, 3);

  EXPECT_CALL(marketServiceMock_, getOwnedAssetValues(ApiGateway::AssetSymbol{"USDT"}))
      .Times(3)
      .WillRepeatedly(Return(MarketService::AssetValues{
          {MarketService::SingleAssetValue{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::AssetSymbol{"USDT"},
                                           ApiGateway::Value{"500.0"}},
           MarketService::SingleAssetValue{ApiGateway::AssetSymbol{"ETH"}, ApiGateway::AssetSymbol{"USDT"},
                                           ApiGateway::Value{"500.0"}}}}));

  EXPECT_CALL(marketServiceMock_, makeMarketTypeOrderWithQuoteQuantity(btcUsdtTradingPair, _, _))
      .Times(3)
      .WillRepeatedly([](const ApiGateway::TradingPairSymbol&, const ApiGateway::OrderSide& side,
                         const ApiGateway::AssetQuantity& quantity) {
        EXPECT_EQ(ApiGateway::OrderSide::Buy, side);
        EXPECT_NEAR(300.0, std::stod(quantity.val_), 0.001);
        return ApiGateway::OrderResult::Success;
      });

  EXPECT_CALL(marketServiceMock_, makeMarketTypeOrderWithQuoteQuantity(ethUsdtTradingPair, _, _))
      .Times(3)
      .WillRepeatedly([](const ApiGateway::TradingPairSymbol&, const ApiGateway::OrderSide& side,
                         const ApiGateway::AssetQuantity& quantity) {
        EXPECT_EQ(ApiGateway::OrderSide::Sell, side);
        EXPECT_NEAR(300.0, std::stod(quantity.val_), 0.001);
        return ApiGateway::OrderResult::Success;
      });

  expectSleep(std::chrono::seconds(config.executionPeriod.val_), 3);
  getSut(config).run(token);
}

}  // namespace BotAlgorithms::Rebalancer
