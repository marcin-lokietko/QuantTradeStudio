#include "ApiGateway/ApiGateway.hpp"

#include <ranges>

#include "GeneratedMocks/BotExecutionMock.hpp"
#include "GeneratedMocks/MarketServiceMock.hpp"
#include "GeneratedMocks/WalletMock.hpp"
#include "gmock/gmock.h"

namespace ApiGateway {

using MarketService::AssetPrices;
using MarketService::SingleAssetPrice;
using MarketService::TradingPairs;
using testing::_;
using testing::Return;

class ApiGatewayTest : public ::testing::Test {
 public:
  testing::StrictMock<MarketService::MarketServiceMock> marketServiceMock_;
  testing::StrictMock<Wallet::WalletMock> walletMock_;
  testing::StrictMock<BotExecution::BotExecutionMock> botExecution_;

  ApiGateway sut_{marketServiceMock_, walletMock_, botExecution_};
};

TEST_F(ApiGatewayTest, WhenGetOwnedAssetsCalled_ThenCallDelegatedToWallet) {
  Assets assets{SingleAsset{
                    AssetSymbol{"BTC"},
                    AssetQuantity{"1"},
                    Value{"80000"},
                },
                SingleAsset{AssetSymbol{"ETH"}, AssetQuantity{"1"}, Value{"2000"}}};
  EXPECT_CALL(walletMock_, getOwnedAssets()).WillOnce(Return(assets));
  EXPECT_EQ(assets, sut_.getOwnedAssets());
}

TEST_F(ApiGatewayTest, WhenMakeOrderCalled_ThenCallDelegatedToMarketService) {
  const AssetSymbol base{"BTC"};
  const AssetSymbol quote{"USDT"};
  const OrderSide side = OrderSide::Buy;
  const AssetQuantity qty{"1.0"};
  const TradingPairSymbol pair{"BTCUSDT"};
  const Price price{"10000"};
  const OrderResult expectedResult = OrderResult::Success;

  EXPECT_CALL(marketServiceMock_, getPrice(pair)).WillOnce(Return(price));
  EXPECT_CALL(marketServiceMock_, makeOrder(pair, side, qty, price)).WillOnce(Return(expectedResult));
  EXPECT_EQ(expectedResult, sut_.makeOrder(base, quote, side, qty));
}

TEST_F(ApiGatewayTest, WhenGetOpenOrdersCalled_ThenCallDelegatedToMarketService) {
  Orders openOrders{SingleOrder{TradingPairSymbol{"BTCUSDT"}, OrderId{1}, Price{"10000"}, AssetQuantity{"0.1"},
                                AssetQuantity{"0.05"}, OrderSide::Buy},
                    SingleOrder{TradingPairSymbol{"ETHUSDT"}, OrderId{2}, Price{"2000"}, AssetQuantity{"0.5"},
                                AssetQuantity{"0.25"}, OrderSide::Sell}};

  EXPECT_CALL(marketServiceMock_, getOpenOrders()).WillOnce(Return(openOrders));
  EXPECT_EQ(openOrders, sut_.getOpenOrders());
}

TEST_F(ApiGatewayTest, WhenGetAvailableQuoteAssetsCalled_ThenTradingPairsWithGivenBaseAssetAreReturnedWithPrices) {
  const AssetSymbol base{"BTC"};
  const MarketService::TradingPairs tradingPairs = {{TradingPairSymbol{"BTCUSDT"}, base, AssetSymbol{"USDT"}},
                                                    {TradingPairSymbol{"BTCEUR"}, base, AssetSymbol{"EUR"}}};

  const std::vector<TradingPairSymbol> symbols = {TradingPairSymbol{"BTCUSDT"}, TradingPairSymbol{"BTCEUR"}};
  MarketService::AssetPrices assetPrices = {SingleAssetPrice{TradingPairSymbol{"BTCUSDT"}, Price{"100000"}},
                                            SingleAssetPrice{TradingPairSymbol{"BTCEUR"}, Price{"90000"}}};

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
  TradingPairs tradingPairs = {{TradingPairSymbol{"BTCUSDT"}, AssetSymbol{"BTC"}, quote},
                               {TradingPairSymbol{"ETHUSDT"}, AssetSymbol{"ETH"}, quote}};
  EXPECT_CALL(marketServiceMock_, getTradingPairsWithQuoteAsset(quote)).WillOnce(Return(tradingPairs));

  auto result = sut_.getAvailableBaseAssets(quote);
  ASSERT_EQ(result.size(), 2);
  EXPECT_TRUE(std::ranges::is_permutation(result, std::vector{AssetSymbol{"BTC"}, AssetSymbol{"ETH"}}));
}

TEST_F(ApiGatewayTest, WhenGetAvailableBaseAssetsCalledWithoutQuoteAsset_ThenAllBaseAssetsAreReturned) {
  TradingPairs tradingPairs = {{TradingPairSymbol{"BTCUSDT"}, AssetSymbol{"BTC"}, AssetSymbol{"USDT"}},
                               {TradingPairSymbol{"ETHEUR"}, AssetSymbol{"ETH"}, AssetSymbol{"EUR"}}};
  EXPECT_CALL(marketServiceMock_, getAllTradingPairs()).WillOnce(Return(tradingPairs));

  auto result = sut_.getAvailableBaseAssets(std::nullopt);
  ASSERT_EQ(result.size(), 2);
  EXPECT_TRUE(std::ranges::is_permutation(result, std::vector{AssetSymbol{"BTC"}, AssetSymbol{"ETH"}}));
}

TEST_F(ApiGatewayTest, WhenGetQuoteAssetsSuitableForRebalancingCalled_ThenAssetsWithAtLeastTwoPairsAreReturned) {
  MarketService::TradingPairs tradingPairs = {
      {.symbol = TradingPairSymbol{"BTCEUR"}, .baseAsset = AssetSymbol{"BTC"}, .quoteAsset = AssetSymbol{"EUR"}},
      {.symbol = TradingPairSymbol{"BTCUSDT"}, .baseAsset = AssetSymbol{"BTC"}, .quoteAsset = AssetSymbol{"USDT"}},
      {.symbol = TradingPairSymbol{"ETHUSDT"}, .baseAsset = AssetSymbol{"ETH"}, .quoteAsset = AssetSymbol{"USDT"}},
      {.symbol = TradingPairSymbol{"BTCPLN"}, .baseAsset = AssetSymbol{"BTC"}, .quoteAsset = AssetSymbol{"PLN"}},
      {.symbol = TradingPairSymbol{"ETHPLN"}, .baseAsset = AssetSymbol{"ETH"}, .quoteAsset = AssetSymbol{"PLN"}},
      {.symbol = TradingPairSymbol{"LTCPLN"}, .baseAsset = AssetSymbol{"LTC"}, .quoteAsset = AssetSymbol{"PLN"}}};
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

}  // namespace ApiGateway
