#include "BotBacktester/Simulators/MarketServiceSimulator.hpp"

#include "BotBacktester/Simulators/SystemTimeSimulator.hpp"
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

const ApiGateway::TransactionFeePercent transactionFeePercent{1};

const ApiGateway::AssetQuantities initialOwnedAssets = {
    {AssetSymbol{"USDT"}, ApiGateway::AssetQuantity{"100000"}, ApiGateway::MarketId::Unknown},
    {AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"1"}, ApiGateway::MarketId::Unknown},
    {AssetSymbol{"ETH"}, ApiGateway::AssetQuantity{"10"}, ApiGateway::MarketId::Unknown},
};

class MarketServiceSimulatorTest : public ::testing::Test {
 public:
  SystemTimeSimulator timeSimulator_{time0, time3};
  MarketServiceSimulator sut_{klines, timeSimulator_, transactionFeePercent, initialOwnedAssets};
};

TEST_F(MarketServiceSimulatorTest, WhenNoTradesDone_ThenSutReturnsSingleElementInOwnedAssetsHistory) {
  ASSERT_EQ(1, sut_.getOwnedAssetsHistory().size());

  auto expectSingleOwnedAssetsRecord = [&sut_ = sut_]() {
    const BotAssetsHistory expectedHistory = {{time0, initialOwnedAssets}};
    EXPECT_EQ(expectedHistory, sut_.getOwnedAssetsHistory());
  };

  expectSingleOwnedAssetsRecord();

  timeSimulator_.sleepFor(std::stop_token{}, std::chrono::minutes(1));
  expectSingleOwnedAssetsRecord();

  timeSimulator_.sleepFor(std::stop_token{}, std::chrono::minutes(1));
  expectSingleOwnedAssetsRecord();

  timeSimulator_.sleepFor(std::stop_token{}, std::chrono::minutes(1));
  expectSingleOwnedAssetsRecord();
}

TEST_F(MarketServiceSimulatorTest, WhenNoTradesDone_ThenSutReturnsAssetsValuesBasedOnKlines) {
  const MarketService::AssetValues expectedValues0 = {
      {AssetSymbol{"USDT"}, AssetSymbol{"USDT"}, ApiGateway::Value{"100000"}},
      {AssetSymbol{"BTC"}, AssetSymbol{"USDT"}, ApiGateway::Value{"80000.000000"}},
      {AssetSymbol{"ETH"}, AssetSymbol{"USDT"}, ApiGateway::Value{"20000.000000"}},
  };
  EXPECT_EQ(expectedValues0, sut_.getOwnedAssetValues(AssetSymbol{"USDT"}));

  timeSimulator_.sleepFor(std::stop_token{}, std::chrono::minutes(1));
  EXPECT_EQ(expectedValues0, sut_.getOwnedAssetValues(AssetSymbol{"USDT"}));

  timeSimulator_.sleepFor(std::stop_token{}, std::chrono::seconds(1));
  const MarketService::AssetValues expectedValues1 = {
      {AssetSymbol{"USDT"}, AssetSymbol{"USDT"}, ApiGateway::Value{"100000"}},
      {AssetSymbol{"BTC"}, AssetSymbol{"USDT"}, ApiGateway::Value{"90000.000000"}},
      {AssetSymbol{"ETH"}, AssetSymbol{"USDT"}, ApiGateway::Value{"30000.000000"}},
  };
  EXPECT_EQ(expectedValues1, sut_.getOwnedAssetValues(AssetSymbol{"USDT"}));

  timeSimulator_.sleepFor(std::stop_token{}, std::chrono::minutes(1));
  const MarketService::AssetValues expectedValues2 = {
      {AssetSymbol{"USDT"}, AssetSymbol{"USDT"}, ApiGateway::Value{"100000"}},
      {AssetSymbol{"BTC"}, AssetSymbol{"USDT"}, ApiGateway::Value{"100000.000000"}},
      {AssetSymbol{"ETH"}, AssetSymbol{"USDT"}, ApiGateway::Value{"40000.000000"}},
  };
  EXPECT_EQ(expectedValues2, sut_.getOwnedAssetValues(AssetSymbol{"USDT"}));
}

TEST_F(MarketServiceSimulatorTest, WhenTradesPerformed_ThenSutReturnsAssetsValuesBasedOnKlinesAndOwnedAmount) {
  const MarketService::AssetValues expectedValues0 = {
      {AssetSymbol{"USDT"}, AssetSymbol{"USDT"}, ApiGateway::Value{"100000"}},
      {AssetSymbol{"BTC"}, AssetSymbol{"USDT"}, ApiGateway::Value{"80000.000000"}},
      {AssetSymbol{"ETH"}, AssetSymbol{"USDT"}, ApiGateway::Value{"20000.000000"}},
  };
  EXPECT_EQ(expectedValues0, sut_.getOwnedAssetValues(AssetSymbol{"USDT"}));

  // 1st trade
  sut_.makeMarketTypeOrderWithQuoteQuantity(btcUsdtTradingPair, ApiGateway::OrderSide::Sell,
                                            ApiGateway::AssetQuantity{"8000"});

  const MarketService::AssetValues expectedValues1 = {
      {AssetSymbol{"USDT"}, AssetSymbol{"USDT"}, ApiGateway::Value{"107920.000000"}},  // 100000 + 0.99*8000 = 107920
      {AssetSymbol{"BTC"}, AssetSymbol{"USDT"}, ApiGateway::Value{"72000.000000"}},
      {AssetSymbol{"ETH"}, AssetSymbol{"USDT"}, ApiGateway::Value{"20000.000000"}},
  };
  EXPECT_EQ(expectedValues1, sut_.getOwnedAssetValues(AssetSymbol{"USDT"}));

  // 2nd trade
  sut_.makeMarketTypeOrderWithQuoteQuantity(ethUsdtTradingPair, ApiGateway::OrderSide::Buy,
                                            ApiGateway::AssetQuantity{"7920"});

  const MarketService::AssetValues expectedValues2 = {
      {AssetSymbol{"USDT"}, AssetSymbol{"USDT"}, ApiGateway::Value{"100000.000000"}},
      {AssetSymbol{"BTC"}, AssetSymbol{"USDT"}, ApiGateway::Value{"72000.000000"}},
      {AssetSymbol{"ETH"}, AssetSymbol{"USDT"}, ApiGateway::Value{"27840.800000"}},  // 20000 + 0.99*7920 = 27840.8
  };
  EXPECT_EQ(expectedValues2, sut_.getOwnedAssetValues(AssetSymbol{"USDT"}));

  // Advance time to next kline, no trade is done, but values reflect new prices
  timeSimulator_.sleepFor(std::stop_token{}, std::chrono::minutes(1) + std::chrono::seconds(1));
  const MarketService::AssetValues expectedValues3 = {
      {AssetSymbol{"USDT"}, AssetSymbol{"USDT"}, ApiGateway::Value{"100000.000000"}},
      {AssetSymbol{"BTC"}, AssetSymbol{"USDT"}, ApiGateway::Value{"81000.000000"}},  // 72000*(90/80)
      {AssetSymbol{"ETH"}, AssetSymbol{"USDT"}, ApiGateway::Value{"41761.200000"}},  // 27840.8*(3/2)
  };
  EXPECT_EQ(expectedValues3, sut_.getOwnedAssetValues(AssetSymbol{"USDT"}));

  // 3rd trade
  sut_.makeMarketTypeOrderWithQuoteQuantity(ethUsdtTradingPair, ApiGateway::OrderSide::Buy,
                                            ApiGateway::AssetQuantity{"10000"});
  const MarketService::AssetValues expectedValues4 = {
      {AssetSymbol{"USDT"}, AssetSymbol{"USDT"}, ApiGateway::Value{"90000.000000"}},
      {AssetSymbol{"BTC"}, AssetSymbol{"USDT"}, ApiGateway::Value{"81000.000000"}},
      {AssetSymbol{"ETH"}, AssetSymbol{"USDT"}, ApiGateway::Value{"51661.200000"}},  // 41761.2 + 0.99*10000 = 51661.2
  };
  EXPECT_EQ(expectedValues4, sut_.getOwnedAssetValues(AssetSymbol{"USDT"}));
}

TEST_F(MarketServiceSimulatorTest, WhenTradesPerformed_ThenSutReturnsOwnedAssetsHistoryBasedOnKlinesAndOwnedAmount) {
  // 1st trade
  sut_.makeMarketTypeOrderWithQuoteQuantity(btcUsdtTradingPair, ApiGateway::OrderSide::Sell,
                                            ApiGateway::AssetQuantity{"8000"});

  // 2nd trade
  sut_.makeMarketTypeOrderWithQuoteQuantity(ethUsdtTradingPair, ApiGateway::OrderSide::Buy,
                                            ApiGateway::AssetQuantity{"7920"});

  // Advance time to next kline, no trade is done, but values reflect new prices
  timeSimulator_.sleepFor(std::stop_token{}, std::chrono::minutes(1) + std::chrono::seconds(1));

  // 3rd trade
  sut_.makeMarketTypeOrderWithQuoteQuantity(ethUsdtTradingPair, ApiGateway::OrderSide::Buy,
                                            ApiGateway::AssetQuantity{"10000"});

  const ApiGateway::AssetQuantities ownedAssetsAfter1stTrade = {
      {AssetSymbol{"USDT"}, ApiGateway::AssetQuantity{"107920.000000"}, ApiGateway::MarketId::Unknown},
      {AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"0.900000"}, ApiGateway::MarketId::Unknown},
      {AssetSymbol{"ETH"}, ApiGateway::AssetQuantity{"10"}, ApiGateway::MarketId::Unknown},
  };

  const ApiGateway::AssetQuantities ownedAssetsAfter2ndTrade = {
      {AssetSymbol{"USDT"}, ApiGateway::AssetQuantity{"100000.000000"}, ApiGateway::MarketId::Unknown},
      {AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"0.900000"}, ApiGateway::MarketId::Unknown},
      {AssetSymbol{"ETH"}, ApiGateway::AssetQuantity{"13.920400"},
       ApiGateway::MarketId::Unknown},  // 10 + 0.99*7920/2000 = 13.9204
  };

  const ApiGateway::AssetQuantities ownedAssetsAfter3rdTrade = {
      {AssetSymbol{"USDT"}, ApiGateway::AssetQuantity{"90000.000000"}, ApiGateway::MarketId::Unknown},
      {AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"0.900000"}, ApiGateway::MarketId::Unknown},
      {AssetSymbol{"ETH"}, ApiGateway::AssetQuantity{"17.220400"},
       ApiGateway::MarketId::Unknown},  // 13.9204 + 0.99*10000/3000 = 17.2204
  };

  const BotAssetsHistory expectedHistory = {
      {time0, initialOwnedAssets},
      {time0, ownedAssetsAfter1stTrade},
      {time0, ownedAssetsAfter2ndTrade},
      {time0 + std::chrono::minutes(1) + std::chrono::seconds(1), ownedAssetsAfter3rdTrade}};
  EXPECT_EQ(expectedHistory, sut_.getOwnedAssetsHistory());
}

TEST_F(MarketServiceSimulatorTest, WhenGetOpenOrdersCalled_ThenEmptyOrdersReturned) {
  EXPECT_EQ(0, sut_.getOpenOrders().size());
}

TEST_F(MarketServiceSimulatorTest, WhenCancelAllOrdersOnASymbolCalled_ThenSuccessReturned) {
  EXPECT_EQ(ApiGateway::OrderResult::Success, sut_.cancelAllOrdersOnASymbol(btcUsdtTradingPair));
}

}  // namespace BotBacktester::Simulators
