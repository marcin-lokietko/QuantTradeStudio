#include "BotAlgorithms/MovingAverageCrossover/MovingAverageCrossover.hpp"

#include <ranges>

#include "GeneratedMocks/MarketServiceMock.hpp"
#include "GeneratedMocks/TimeMock.hpp"
#include "gmock/gmock.h"

namespace BotAlgorithms::MovingAverageCrossover {

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
const TradingPairSymbol slnUsdtTradingPair{AssetSymbol{"SLN"}, AssetSymbol{"USDT"}};

const Config config{.executionPeriod = ExecutionPeriod{60},
                    .shortTermMovingAverageLength = ApiGateway::MovingAverageLength{5},
                    .longTermMovingAverageLength = ApiGateway::MovingAverageLength{10},
                    .quoteAsset = AssetSymbol{"USDT"},
                    .baseAssets = {AssetSymbol{"BTC"}, AssetSymbol{"ETH"}}};

const ApiGateway::SingleAssetQuantity ownedUsdt{AssetSymbol{"USDT"}, ApiGateway::AssetQuantity{"1000"}};
const ApiGateway::SingleAssetQuantity ownedBtc{AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"1"}};
const ApiGateway::AssetQuantities ownedAssetQuantities{
    ownedUsdt, ownedBtc, ApiGateway::SingleAssetQuantity{AssetSymbol{"ETH"}, ApiGateway::AssetQuantity{"10"}}};

std::vector<MarketService::AssetPrices> buildBullishBtcPriceSequence() {
  const MarketService::SingleAssetPrice constEthPrice{ethUsdtTradingPair, ApiGateway::Price{"1000"}};

  std::vector<MarketService::AssetPrices> assetPricesSequence;
  for (int i = 0; i < 10; ++i) {
    assetPricesSequence.push_back(
        {MarketService::SingleAssetPrice{btcUsdtTradingPair, ApiGateway::Price{std::to_string(100000 - i)}},
         constEthPrice});
  }
  // At this point:
  // short-term (5) BTC MA = 99993
  // long-term (10) BTC MA = 99995.5
  assetPricesSequence.push_back(
      {MarketService::SingleAssetPrice{btcUsdtTradingPair, ApiGateway::Price{"101000"}}, constEthPrice});
  // At this point:
  // short-term (5) BTC MA = 100194
  // long-term (10) BTC MA = 100095.5
  // Crossover happened - buy signal should be emmited for BTC

  return assetPricesSequence;
}

std::vector<MarketService::AssetPrices> buildBearishBtcPriceSequence() {
  const MarketService::SingleAssetPrice constEthPrice{ethUsdtTradingPair, ApiGateway::Price{"1000"}};

  std::vector<MarketService::AssetPrices> assetPricesSequence;
  for (int i = 0; i < 10; ++i) {
    assetPricesSequence.push_back(
        {MarketService::SingleAssetPrice{btcUsdtTradingPair, ApiGateway::Price{std::to_string(100000 + i)}},
         constEthPrice});
  }
  // At this point:
  // short-term (5) BTC MA = 100007
  // long-term (10) BTC MA = 100004.5
  assetPricesSequence.push_back(
      {MarketService::SingleAssetPrice{btcUsdtTradingPair, ApiGateway::Price{"99000"}}, constEthPrice});
  // At this point:
  // short-term (5) BTC MA = 99806
  // long-term (10) BTC MA = 99985.1

  return assetPricesSequence;
}

class MovingAverageCrossoverTest : public ::testing::Test {
 public:
  void expectSleep(std::chrono::seconds expectedDelay, uint64_t expectedNumCalls) {
    EXPECT_CALL(timeMock_, sleepFor(_, std::chrono::duration_cast<std::chrono::milliseconds>(expectedDelay)))
        .Times(expectedNumCalls)
        .WillRepeatedly(Return());
  }

  void expectPriceChecks(const std::vector<MarketService::AssetPrices>& assetPricesSequence) {
    expectSleep(std::chrono::seconds(config.executionPeriod.val_), assetPricesSequence.size());

    auto numCallsSoFar = std::make_shared<int>(0);

    std::vector<ApiGateway::TradingPairSymbol> symbols;
    for (const auto& assetSymbol : config.baseAssets) {
      symbols.push_back(TradingPairSymbol{assetSymbol, config.quoteAsset});
    }

    const int numCallsUntilRequestStopSimulated = static_cast<int>(assetPricesSequence.size());

    EXPECT_CALL(marketServiceMock_, getPrices(symbols))
        .WillRepeatedly([source = &this->source, &assetPricesSequence, numCallsSoFar,
                         numCallsUntilRequestStopSimulated](const auto&) {
          if (++(*numCallsSoFar) >= numCallsUntilRequestStopSimulated) {
            source->request_stop();
          }
          return assetPricesSequence.at(*numCallsSoFar - 1);
        });
  }

  MovingAverageCrossover getSut(Config config) { return {std::move(config), marketServiceMock_, timeMock_}; }

  testing::StrictMock<MarketService::MarketServiceMock> marketServiceMock_;
  testing::StrictMock<Time::TimeMock> timeMock_;

  std::stop_source source;
  std::stop_token token = source.get_token();
};

TEST_F(MovingAverageCrossoverTest, WhenMinimalAssetPriceHistoryNotReached_ThenBotDoesNotTrade) {
  const std::vector<MarketService::AssetPrices> assetPricesSequence(
      10, {MarketService::SingleAssetPrice{btcUsdtTradingPair, ApiGateway::Price{"100000"}},
           MarketService::SingleAssetPrice{ethUsdtTradingPair, ApiGateway::Price{"1000"}}});

  expectPriceChecks(assetPricesSequence);
  getSut(config).run(token);
}

TEST_F(MovingAverageCrossoverTest, WhenPricesChangeLinearly_ThenBotDoesNotTrade) {
  std::vector<MarketService::AssetPrices> assetPricesSequence;
  for (int i = 0; i < 50; ++i) {
    assetPricesSequence.push_back(
        {MarketService::SingleAssetPrice{btcUsdtTradingPair, ApiGateway::Price{std::to_string(100000 + i * 10)}},
         MarketService::SingleAssetPrice{ethUsdtTradingPair, ApiGateway::Price{std::to_string(1000 - i)}}});
  }

  expectPriceChecks(assetPricesSequence);
  getSut(config).run(token);
}

TEST_F(MovingAverageCrossoverTest,
       WhenShortTermAverageStartsBeingGreaterThanLongTerm_ThenBotMakesBuyOrderForAllBaseAssets) {
  const auto assetPricesSequence = buildBullishBtcPriceSequence();

  expectPriceChecks(assetPricesSequence);

  EXPECT_CALL(marketServiceMock_, getOpenOrders()).WillOnce(Return(ApiGateway::Orders{}));
  EXPECT_CALL(marketServiceMock_, getOwnedAssetsQuantity()).WillOnce(Return(ownedAssetQuantities));
  EXPECT_CALL(marketServiceMock_, makeMarketTypeOrderWithQuoteQuantity(btcUsdtTradingPair, ApiGateway::OrderSide::Buy,
                                                                       ownedUsdt.freeQuantity))
      .WillOnce(Return(ApiGateway::OrderResult::Success));

  getSut(config).run(token);
}

TEST_F(MovingAverageCrossoverTest, WhenOrderDone_ThenCurrentOrdersOnRelevantAssetsAreCancelled) {
  const auto assetPricesSequence = buildBullishBtcPriceSequence();

  expectPriceChecks(assetPricesSequence);

  ApiGateway::Orders openOrders{ApiGateway::SingleOrder{btcUsdtTradingPair, ApiGateway::OrderId{1},
                                                        ApiGateway::Price{"10000"}, ApiGateway::AssetQuantity{"0.1"},
                                                        ApiGateway::AssetQuantity{"0.05"}, ApiGateway::OrderSide::Buy},
                                ApiGateway::SingleOrder{ethUsdtTradingPair, ApiGateway::OrderId{2},
                                                        ApiGateway::Price{"2000"}, ApiGateway::AssetQuantity{"0.5"},
                                                        ApiGateway::AssetQuantity{"0.25"}, ApiGateway::OrderSide::Sell},
                                ApiGateway::SingleOrder{slnUsdtTradingPair, ApiGateway::OrderId{3},
                                                        ApiGateway::Price{"5"}, ApiGateway::AssetQuantity{"100"},
                                                        ApiGateway::AssetQuantity{"50"}, ApiGateway::OrderSide::Buy}};

  EXPECT_CALL(marketServiceMock_, getOpenOrders()).WillOnce(Return(openOrders));
  EXPECT_CALL(marketServiceMock_, cancelAllOrdersOnASymbol(btcUsdtTradingPair))
      .WillOnce(Return(ApiGateway::OrderResult::Success));
  EXPECT_CALL(marketServiceMock_, cancelAllOrdersOnASymbol(ethUsdtTradingPair))
      .WillOnce(Return(ApiGateway::OrderResult::Success));
  EXPECT_CALL(marketServiceMock_, cancelAllOrdersOnASymbol(slnUsdtTradingPair)).Times(0);

  EXPECT_CALL(marketServiceMock_, getOwnedAssetsQuantity()).WillOnce(Return(ownedAssetQuantities));
  EXPECT_CALL(marketServiceMock_, makeMarketTypeOrderWithQuoteQuantity(btcUsdtTradingPair, ApiGateway::OrderSide::Buy,
                                                                       ownedUsdt.freeQuantity))
      .WillOnce(Return(ApiGateway::OrderResult::Success));

  getSut(config).run(token);
}

TEST_F(MovingAverageCrossoverTest,
       WhenShortTermAverageStartsBeingLowerThanLongTerm_ThenBotMakesSellsCorrespondingAsset) {
  const auto assetPricesSequence = buildBearishBtcPriceSequence();

  expectPriceChecks(assetPricesSequence);

  EXPECT_CALL(marketServiceMock_, getOpenOrders()).WillOnce(Return(ApiGateway::Orders{}));
  EXPECT_CALL(marketServiceMock_, getOwnedAssetsQuantity()).WillOnce(Return(ownedAssetQuantities));
  EXPECT_CALL(marketServiceMock_, makeOrder(btcUsdtTradingPair, ApiGateway::OrderSide::Sell, ownedBtc.freeQuantity,
                                            testing::Eq(std::nullopt)))
      .WillOnce(Return(ApiGateway::OrderResult::Success));

  getSut(config).run(token);
}

}  // namespace BotAlgorithms::MovingAverageCrossover
