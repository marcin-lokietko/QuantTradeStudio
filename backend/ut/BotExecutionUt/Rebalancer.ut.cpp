#include "BotExecution/Rebalancer/Rebalancer.hpp"

#include "GeneratedMocks/MarketServiceMock.hpp"
#include "GeneratedMocks/TimeMock.hpp"
#include "GeneratedMocks/WalletMock.hpp"
#include "gmock/gmock.h"

namespace BotExecution::Rebalancer {

using ApiGateway::AssetSymbol;
using ApiGateway::ExecutionPeriod;
using ApiGateway::IsExecutedImmediately;
using ApiGateway::SharePercent;
using ApiGateway::SingleAssetShare;
using testing::_;
using testing::Return;

const ApiGateway::AssetShares assetShares{SingleAssetShare{AssetSymbol{"BTC"}, SharePercent{80}},
                                          SingleAssetShare{AssetSymbol{"ETH"}, SharePercent{20}}};

const Config config{.executionPeriod = ExecutionPeriod{60},
                    .isExecutedImmediately = IsExecutedImmediately{false},
                    .quoteAsset = AssetSymbol{"USDT"},
                    .baseAssetShares = assetShares};

class RebalancerTest : public ::testing::Test {
 public:
  std::unique_ptr<Time::TimeMock> getTimeMock(std::chrono::seconds expectedDelay, uint64_t expectedNumCalls = 1) {
    auto time = std::make_unique<Time::TimeMock>();
    EXPECT_CALL(*time, sleepFor(_, std::chrono::duration_cast<std::chrono::milliseconds>(expectedDelay)))
        .Times(expectedNumCalls)
        .WillRepeatedly(Return());
    return time;
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

  Rebalancer getSut(Config config, std::unique_ptr<Time::TimeMock> time) {
    return {std::move(config), marketServiceMock_, walletMock_, std::move(time)};
  }

  testing::StrictMock<MarketService::MarketServiceMock> marketServiceMock_;
  testing::StrictMock<Wallet::WalletMock> walletMock_;

  std::stop_source source;
  std::stop_token token = source.get_token();
};

TEST_F(RebalancerTest, WhenConfiguredBaseAssetsNowOwned_ThenBotDoesNotTrade) {
  expectGetOpenOrdersCalls(ApiGateway::Orders{}, 1);

  EXPECT_CALL(walletMock_, getOwnedAssetValues(ApiGateway::AssetSymbol{"USDT"}))
      .WillOnce(Return(Wallet::AssetValues{}));

  getSut(config, getTimeMock(std::chrono::seconds(config.executionPeriod.val_))).run(token);
}

TEST_F(RebalancerTest, WhenOrdersOnConfiguredTradingPairsAreOpen_ThenBotCancelsThemAndOnlyThem) {
  ApiGateway::Orders openOrders{
      ApiGateway::SingleOrder{ApiGateway::TradingPairSymbol{"BTCUSDT"}, ApiGateway::OrderId{1},
                              ApiGateway::Price{"10000"}, ApiGateway::AssetQuantity{"0.1"},
                              ApiGateway::AssetQuantity{"0.05"}, ApiGateway::OrderSide::Buy},
      ApiGateway::SingleOrder{ApiGateway::TradingPairSymbol{"ETHUSDT"}, ApiGateway::OrderId{2},
                              ApiGateway::Price{"2000"}, ApiGateway::AssetQuantity{"0.5"},
                              ApiGateway::AssetQuantity{"0.25"}, ApiGateway::OrderSide::Sell},
      ApiGateway::SingleOrder{ApiGateway::TradingPairSymbol{"ETHPLN"}, ApiGateway::OrderId{3},
                              ApiGateway::Price{"2000"}, ApiGateway::AssetQuantity{"0.5"},
                              ApiGateway::AssetQuantity{"0.25"}, ApiGateway::OrderSide::Sell},
      ApiGateway::SingleOrder{ApiGateway::TradingPairSymbol{"ETHEUR"}, ApiGateway::OrderId{4},
                              ApiGateway::Price{"2000"}, ApiGateway::AssetQuantity{"0.5"},
                              ApiGateway::AssetQuantity{"0.25"}, ApiGateway::OrderSide::Sell}};
  expectGetOpenOrdersCalls(openOrders, 1);

  EXPECT_CALL(marketServiceMock_, cancelAllOrdersOnASymbol(ApiGateway::TradingPairSymbol{"BTCUSDT"}))
      .WillOnce(Return(ApiGateway::OrderResult::Success));
  EXPECT_CALL(marketServiceMock_, cancelAllOrdersOnASymbol(ApiGateway::TradingPairSymbol{"ETHUSDT"}))
      .WillOnce(Return(ApiGateway::OrderResult::Success));

  EXPECT_CALL(walletMock_, getOwnedAssetValues(ApiGateway::AssetSymbol{"USDT"}))
      .WillOnce(Return(Wallet::AssetValues{}));

  getSut(config, getTimeMock(std::chrono::seconds(config.executionPeriod.val_))).run(token);
}

TEST_F(RebalancerTest, WhenConfiguredBaseAssetsHaveInvalidValues_ThenBotDoesNotTrade) {
  expectGetOpenOrdersCalls(ApiGateway::Orders{}, 1);

  EXPECT_CALL(walletMock_, getOwnedAssetValues(ApiGateway::AssetSymbol{"USDT"}))
      .WillOnce(Return(
          Wallet::AssetValues{{Wallet::SingleAssetValue{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::AssetSymbol{"USDT"},
                                                        ApiGateway::Value{"invalid_value"}},
                               Wallet::SingleAssetValue{ApiGateway::AssetSymbol{"ETH"}, ApiGateway::AssetSymbol{"USDT"},
                                                        ApiGateway::Value{"100.5"}}

          }}));

  getSut(config, getTimeMock(std::chrono::seconds(config.executionPeriod.val_))).run(token);
}

TEST_F(RebalancerTest, WhenOwnedAssetsSharedAreCloseToExpected_ThenBotDoesNotTrade) {
  expectGetOpenOrdersCalls(ApiGateway::Orders{}, 1);

  EXPECT_CALL(walletMock_, getOwnedAssetValues(ApiGateway::AssetSymbol{"USDT"}))
      .WillOnce(Return(
          Wallet::AssetValues{{Wallet::SingleAssetValue{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::AssetSymbol{"USDT"},
                                                        ApiGateway::Value{"80002.123"}},
                               Wallet::SingleAssetValue{ApiGateway::AssetSymbol{"ETH"}, ApiGateway::AssetSymbol{"USDT"},
                                                        ApiGateway::Value{"20001.321"}}}}));

  getSut(config, getTimeMock(std::chrono::seconds(config.executionPeriod.val_))).run(token);
}

TEST_F(RebalancerTest, WhenOwnedAssetsSharedDifferentThanExpected_ThenOrdersArePlaced) {
  expectGetOpenOrdersCalls(ApiGateway::Orders{}, 1);

  EXPECT_CALL(walletMock_, getOwnedAssetValues(ApiGateway::AssetSymbol{"USDT"}))
      .WillOnce(Return(
          Wallet::AssetValues{{Wallet::SingleAssetValue{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::AssetSymbol{"USDT"},
                                                        ApiGateway::Value{"500.0"}},
                               Wallet::SingleAssetValue{ApiGateway::AssetSymbol{"ETH"}, ApiGateway::AssetSymbol{"USDT"},
                                                        ApiGateway::Value{"500.0"}}}}));

  EXPECT_CALL(marketServiceMock_, makeMarketTypeOrderWithQuoteQuantity(ApiGateway::TradingPairSymbol{"BTCUSDT"}, _, _))
      .WillOnce([](const ApiGateway::TradingPairSymbol&, const ApiGateway::OrderSide& side,
                   const ApiGateway::AssetQuantity& quantity) {
        EXPECT_EQ(ApiGateway::OrderSide::Buy, side);
        EXPECT_NEAR(300.0, std::stod(quantity.val_), 0.001);
        return ApiGateway::OrderResult::Success;
      });

  EXPECT_CALL(marketServiceMock_, makeMarketTypeOrderWithQuoteQuantity(ApiGateway::TradingPairSymbol{"ETHUSDT"}, _, _))
      .WillOnce([](const ApiGateway::TradingPairSymbol&, const ApiGateway::OrderSide& side,
                   const ApiGateway::AssetQuantity& quantity) {
        EXPECT_EQ(ApiGateway::OrderSide::Sell, side);
        EXPECT_NEAR(300.0, std::stod(quantity.val_), 0.001);
        return ApiGateway::OrderResult::Success;
      });

  getSut(config, getTimeMock(std::chrono::seconds(config.executionPeriod.val_))).run(token);
}

TEST_F(RebalancerTest,
       WhenOwnedAssetsSharedDifferentThanExpectedAndBotStoppedAfter3Executions_ThenOrdersArePlaced3Times) {
  expectGetOpenOrdersCalls(ApiGateway::Orders{}, 3);

  EXPECT_CALL(walletMock_, getOwnedAssetValues(ApiGateway::AssetSymbol{"USDT"}))
      .Times(3)
      .WillRepeatedly(Return(
          Wallet::AssetValues{{Wallet::SingleAssetValue{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::AssetSymbol{"USDT"},
                                                        ApiGateway::Value{"500.0"}},
                               Wallet::SingleAssetValue{ApiGateway::AssetSymbol{"ETH"}, ApiGateway::AssetSymbol{"USDT"},
                                                        ApiGateway::Value{"500.0"}}}}));

  EXPECT_CALL(marketServiceMock_, makeMarketTypeOrderWithQuoteQuantity(ApiGateway::TradingPairSymbol{"BTCUSDT"}, _, _))
      .Times(3)
      .WillRepeatedly([](const ApiGateway::TradingPairSymbol&, const ApiGateway::OrderSide& side,
                         const ApiGateway::AssetQuantity& quantity) {
        EXPECT_EQ(ApiGateway::OrderSide::Buy, side);
        EXPECT_NEAR(300.0, std::stod(quantity.val_), 0.001);
        return ApiGateway::OrderResult::Success;
      });

  EXPECT_CALL(marketServiceMock_, makeMarketTypeOrderWithQuoteQuantity(ApiGateway::TradingPairSymbol{"ETHUSDT"}, _, _))
      .Times(3)
      .WillRepeatedly([](const ApiGateway::TradingPairSymbol&, const ApiGateway::OrderSide& side,
                         const ApiGateway::AssetQuantity& quantity) {
        EXPECT_EQ(ApiGateway::OrderSide::Sell, side);
        EXPECT_NEAR(300.0, std::stod(quantity.val_), 0.001);
        return ApiGateway::OrderResult::Success;
      });

  getSut(config, getTimeMock(std::chrono::seconds(config.executionPeriod.val_), 3)).run(token);
}

}  // namespace BotExecution::Rebalancer
