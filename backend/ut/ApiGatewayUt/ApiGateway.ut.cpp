#include "ApiGateway/ApiGateway.hpp"

#include <ranges>

#include "GeneratedMocks/BotBacktesterMock.hpp"
#include "GeneratedMocks/BotExecutionMock.hpp"
#include "GeneratedMocks/MarketServiceMock.hpp"
#include "gmock/gmock.h"

namespace ApiGateway {

using MarketService::AssetPrices;
using MarketService::SingleAssetPrice;
using MarketService::TradingPairs;
using testing::_;
using testing::Return;

const TradingPairSymbol btcUsdtTradingPair{AssetSymbol{"BTC"}, AssetSymbol{"USDT"}};
const TradingPairSymbol ethUsdtTradingPair{AssetSymbol{"ETH"}, AssetSymbol{"USDT"}};
const TradingPairSymbol btcEurTradingPair{AssetSymbol{"BTC"}, AssetSymbol{"EUR"}};
const TradingPairSymbol ethEurTradingPair{AssetSymbol{"ETH"}, AssetSymbol{"EUR"}};
const TradingPairSymbol btcPlnTradingPair{AssetSymbol{"BTC"}, AssetSymbol{"PLN"}};
const TradingPairSymbol ethPlnTradingPair{AssetSymbol{"ETH"}, AssetSymbol{"PLN"}};
const TradingPairSymbol ltcPlnTradingPair{AssetSymbol{"LTC"}, AssetSymbol{"PLN"}};

class ApiGatewayTest : public ::testing::Test {
 public:
  testing::StrictMock<MarketService::MarketServiceMock> marketServiceMock_;
  testing::StrictMock<BotExecution::BotExecutionMock> botExecution_;
  testing::StrictMock<BotBacktester::BotBacktesterMock> botBacktester_;

  ApiGateway sut_{marketServiceMock_, botExecution_, botBacktester_};
};

TEST_F(ApiGatewayTest, WhenGetOwnedAssetsCalled_ThenCallDelegatedToMarketService) {
  AssetValues assets{SingleAssetValue{
                         AssetSymbol{"BTC"},
                         AssetQuantity{"1"},
                         Value{"80000"},
                     },
                     SingleAssetValue{AssetSymbol{"ETH"}, AssetQuantity{"1"}, Value{"2000"}}};
  EXPECT_CALL(marketServiceMock_, getOwnedAssetsQuantityAndValue()).WillOnce(Return(assets));
  EXPECT_EQ(assets, sut_.getOwnedAssets());
}

TEST_F(ApiGatewayTest, WhenMakeOrderCalled_ThenCallDelegatedToMarketService) {
  const AssetSymbol base{"BTC"};
  const AssetSymbol quote{"USDT"};
  const OrderSide side = OrderSide::Buy;
  const AssetQuantity qty{"1.0"};
  const TradingPairSymbol pair{base, quote};
  const Price price{"10000"};
  const OrderResult expectedResult = OrderResult::Success;

  EXPECT_CALL(marketServiceMock_, getPrice(pair)).WillOnce(Return(price));
  EXPECT_CALL(marketServiceMock_, makeOrder(pair, side, qty, std::make_optional<Price>(price)))
      .WillOnce(Return(expectedResult));
  EXPECT_EQ(expectedResult, sut_.makeOrder(base, quote, side, qty));
}

TEST_F(ApiGatewayTest, WhenGetOpenOrdersCalled_ThenCallDelegatedToMarketService) {
  Orders openOrders{SingleOrder{btcUsdtTradingPair, OrderId{1}, Price{"10000"}, AssetQuantity{"0.1"},
                                AssetQuantity{"0.05"}, OrderSide::Buy},
                    SingleOrder{ethUsdtTradingPair, OrderId{2}, Price{"2000"}, AssetQuantity{"0.5"},
                                AssetQuantity{"0.25"}, OrderSide::Sell}};

  EXPECT_CALL(marketServiceMock_, getOpenOrders()).WillOnce(Return(openOrders));
  EXPECT_EQ(openOrders, sut_.getOpenOrders());
}

TEST_F(ApiGatewayTest, WhenGetAvailableQuoteAssetsCalled_ThenTradingPairsWithGivenBaseAssetAreReturnedWithPrices) {
  const AssetSymbol base{"BTC"};
  const MarketService::TradingPairs tradingPairs = {btcUsdtTradingPair, btcEurTradingPair};

  const std::vector<TradingPairSymbol> symbols = {btcUsdtTradingPair, btcEurTradingPair};
  MarketService::AssetPrices assetPrices = {SingleAssetPrice{btcUsdtTradingPair, Price{"100000"}},
                                            SingleAssetPrice{btcEurTradingPair, Price{"90000"}}};

  EXPECT_CALL(marketServiceMock_, getTradingPairsWithBaseAsset(base)).WillOnce(Return(tradingPairs));
  EXPECT_CALL(marketServiceMock_, getPrices(symbols)).WillOnce(Return(assetPrices));

  auto result = sut_.getAvailableQuoteAssets(base);
  ASSERT_EQ(2, result.size());
  EXPECT_TRUE(
      std::ranges::is_permutation(result, std::vector{SingleAvailableQuoteAsset(AssetSymbol{"USDT"}, Price{"100000"}),
                                                      SingleAvailableQuoteAsset(AssetSymbol{"EUR"}, Price{"90000"})}));
}

TEST_F(ApiGatewayTest, WhenGetAvailableBaseAssetsCalledWithQuoteAsset_ThenCorrespondingBaseAssetsAreReturned) {
  const AssetSymbol quote{"USDT"};
  TradingPairs tradingPairs = {btcUsdtTradingPair, ethUsdtTradingPair};
  EXPECT_CALL(marketServiceMock_, getTradingPairsWithQuoteAsset(quote)).WillOnce(Return(tradingPairs));

  auto result = sut_.getAvailableBaseAssets(quote);
  ASSERT_EQ(result.size(), 2);
  EXPECT_TRUE(std::ranges::is_permutation(result, std::vector{AssetSymbol{"BTC"}, AssetSymbol{"ETH"}}));
}

TEST_F(ApiGatewayTest, WhenGetAvailableBaseAssetsCalledWithoutQuoteAsset_ThenAllBaseAssetsAreReturned) {
  TradingPairs tradingPairs = {btcUsdtTradingPair, ethEurTradingPair};
  EXPECT_CALL(marketServiceMock_, getAllTradingPairs()).WillOnce(Return(tradingPairs));

  auto result = sut_.getAvailableBaseAssets(std::nullopt);
  ASSERT_EQ(result.size(), 2);
  EXPECT_TRUE(std::ranges::is_permutation(result, std::vector{AssetSymbol{"BTC"}, AssetSymbol{"ETH"}}));
}

TEST_F(ApiGatewayTest, WhenGetQuoteAssetsSuitableForRebalancingCalled_ThenAssetsWithAtLeastTwoPairsAreReturned) {
  MarketService::TradingPairs tradingPairs = {btcEurTradingPair, btcUsdtTradingPair, ethUsdtTradingPair,
                                              btcPlnTradingPair, ethPlnTradingPair,  ltcPlnTradingPair};
  EXPECT_CALL(marketServiceMock_, getAllTradingPairs()).WillOnce(Return(tradingPairs));
  auto result = sut_.getQuoteAssetsSuitableForRebalancing();

  ASSERT_EQ(2, result.size());
  EXPECT_TRUE(std::ranges::is_permutation(result, std::vector{AssetSymbol{"USDT"}, AssetSymbol{"PLN"}}));
}

TEST_F(ApiGatewayTest, WhenStartBotCalled_ThenCallDelegatedToMarketService) {
  BotConfig config;
  config.botName = BotName{"TestBot"};
  EXPECT_CALL(botExecution_, startBot(_)).WillOnce(Return(StartBotResult::Success));
  EXPECT_EQ(StartBotResult::Success, sut_.startBot(config));
}

TEST_F(ApiGatewayTest, WhenStopAllBotsCalled_ThenCallDelegatedToMarketService) {
  EXPECT_CALL(botExecution_, stopAllBots()).WillOnce(Return(StopAllBotsResult::Success));
  EXPECT_EQ(StopAllBotsResult::Success, sut_.stopAllBots());
}

TEST_F(ApiGatewayTest, WhenTestBotCalled_ThenCallDelegatedToBotBacktester) {
  BacktestResults results;
  results.totalProfitOrLossInAbsolute = AssetQuantity{"1000"};
  EXPECT_CALL(botBacktester_, testBot(_, _)).WillOnce(Return(results));

  const BotConfig botConfig;
  const BacktestConfig backtestConfig;
  EXPECT_EQ(results, sut_.testBot(botConfig, backtestConfig));
}

}  // namespace ApiGateway
