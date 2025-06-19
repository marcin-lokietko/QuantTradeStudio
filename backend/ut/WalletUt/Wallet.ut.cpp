#include "Wallet/Wallet.hpp"

#include "GeneratedMocks/MarketServiceMock.hpp"
#include "gmock/gmock.h"

namespace Wallet {

using testing::_;
using testing::Return;

class WalletTest : public ::testing::Test {
 public:
  testing::StrictMock<MarketService::MarketServiceMock> marketServiceMock_;

  Wallet sut_{marketServiceMock_};
};

TEST_F(WalletTest, WhenGetOwnedAssetsCalled_ThenReturnsAssetsWithCalculatedUSDTValue) {
  const MarketService::TradingPairs tradingPairs = {
      {ApiGateway::TradingPairSymbol{"BTCUSDT"}, ApiGateway::AssetSymbol{"BTC"}, ApiGateway::AssetSymbol{"USDT"}},
      {ApiGateway::TradingPairSymbol{"ETHUSDT"}, ApiGateway::AssetSymbol{"ETH"}, ApiGateway::AssetSymbol{"USDT"}}};
  EXPECT_CALL(marketServiceMock_, getTradingPairsWithQuoteAsset(ApiGateway::AssetSymbol{"USDT"}))
      .WillOnce(Return(tradingPairs));

  const std::vector<ApiGateway::TradingPairSymbol> pairSymbols = {ApiGateway::TradingPairSymbol{"BTCUSDT"},
                                                                  ApiGateway::TradingPairSymbol{"ETHUSDT"}};
  const MarketService::AssetPrices prices = {{ApiGateway::TradingPairSymbol{"BTCUSDT"}, ApiGateway::Price{"100000"}},
                                             {ApiGateway::TradingPairSymbol{"ETHUSDT"}, ApiGateway::Price{"2000"}}};
  EXPECT_CALL(marketServiceMock_, getPrices(pairSymbols)).WillOnce(Return(prices));

  const MarketService::Assets marketAssets = {
      {ApiGateway::AssetSymbol{"BTC"}, MarketService::AssetQuantity{"2"}, MarketService::MarketId::Binance},
      {ApiGateway::AssetSymbol{"ETH"}, MarketService::AssetQuantity{"4"}, MarketService::MarketId::Binance}};
  EXPECT_CALL(marketServiceMock_, getOwnedAssets()).WillOnce(Return(marketAssets));

  auto result = sut_.getOwnedAssets();

  ASSERT_EQ(result.size(), 2);
  EXPECT_EQ(result[0].assetSymbol, ApiGateway::AssetSymbol{"BTC"});
  EXPECT_EQ(result[0].freeQuantity, ApiGateway::AssetQuantity{"2"});
  EXPECT_NEAR(std::stod(result[0].usdtValue.val_), 200000.0, 0.001);
  EXPECT_EQ(result[1].assetSymbol, ApiGateway::AssetSymbol{"ETH"});
  EXPECT_EQ(result[1].freeQuantity, ApiGateway::AssetQuantity{"4"});
  EXPECT_NEAR(std::stod(result[1].usdtValue.val_), 8000.0, 0.001);
}

TEST_F(WalletTest, WhenGetOwnedAssetValuesCalled_ThenReturnsAssetValuesWithCalculatedUSDTValue) {
  const ApiGateway::AssetSymbol quoteAsset{"USDT"};
  const MarketService::TradingPairs tradingPairs = {
      {ApiGateway::TradingPairSymbol{"BTCUSDT"}, ApiGateway::AssetSymbol{"BTC"}, quoteAsset},
      {ApiGateway::TradingPairSymbol{"ETHUSDT"}, ApiGateway::AssetSymbol{"ETH"}, quoteAsset}};

  EXPECT_CALL(marketServiceMock_, getTradingPairsWithQuoteAsset(quoteAsset)).WillOnce(Return(tradingPairs));

  const std::vector<ApiGateway::TradingPairSymbol> pairSymbols = {ApiGateway::TradingPairSymbol{"BTCUSDT"},
                                                                  ApiGateway::TradingPairSymbol{"ETHUSDT"}};
  const MarketService::AssetPrices prices = {{ApiGateway::TradingPairSymbol{"BTCUSDT"}, ApiGateway::Price{"40000"}},
                                             {ApiGateway::TradingPairSymbol{"ETHUSDT"}, ApiGateway::Price{"2500"}}};
  EXPECT_CALL(marketServiceMock_, getPrices(pairSymbols)).WillOnce(Return(prices));

  const MarketService::Assets marketAssets = {
      {ApiGateway::AssetSymbol{"BTC"}, MarketService::AssetQuantity{"1.5"}, MarketService::MarketId::Binance},
      {ApiGateway::AssetSymbol{"ETH"}, MarketService::AssetQuantity{"3"}, MarketService::MarketId::Binance}};
  EXPECT_CALL(marketServiceMock_, getOwnedAssets()).WillOnce(Return(marketAssets));

  auto result = sut_.getOwnedAssetValues(quoteAsset);

  ASSERT_EQ(result.size(), 2);
  EXPECT_EQ(result[0].baseSymbol, ApiGateway::AssetSymbol{"BTC"});
  EXPECT_EQ(result[0].quoteAsset, quoteAsset);
  EXPECT_NEAR(std::stod(result[0].value->val_), 60000.0, 0.001);
  EXPECT_EQ(result[1].baseSymbol, ApiGateway::AssetSymbol{"ETH"});
  EXPECT_EQ(result[1].quoteAsset, quoteAsset);
  EXPECT_NEAR(std::stod(result[1].value->val_), 7500.0, 0.001);
}

}  // namespace Wallet
