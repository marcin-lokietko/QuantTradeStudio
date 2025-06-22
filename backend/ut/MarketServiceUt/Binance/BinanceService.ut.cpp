#include <ranges>

#include "GeneratedMocks/EncryptionMock.hpp"
#include "GeneratedMocks/HttpMock.hpp"
#include "GeneratedMocks/TimeMock.hpp"
#include "MarketService/Binance/BinanceService.hpp"
#include "gmock/gmock.h"

namespace MarketService::Binance {

using ApiGateway::AssetSymbol;
using ApiGateway::TradingPairSymbol;
using testing::_;
using testing::Return;

const TradingPairSymbol btcUsdtTradingPair{AssetSymbol{"BTC"}, AssetSymbol{"USDT"}};
const TradingPairSymbol ethUsdtTradingPair{AssetSymbol{"ETH"}, AssetSymbol{"USDT"}};
const TradingPairSymbol ethEurTradingPair{AssetSymbol{"ETH"}, AssetSymbol{"EUR"}};

const Config::BinanceUrlPrefix dummyBinanceUrlPrefix{"https://binance.com"};
const std::string tradingPairsUrl = dummyBinanceUrlPrefix.val_ + "/exchangeInfo";
const std::string binanceTradingPairsResponse = R"({
    "symbols": [
      {"symbol": "BTCUSDT", "baseAsset": "BTC", "quoteAsset": "USDT"},
      {"symbol": "ETHUSDT", "baseAsset": "ETH", "quoteAsset": "USDT"},
      {"symbol": "ETHEUR", "baseAsset": "ETH", "quoteAsset": "EUR"}
    ]
  })";

class BinanceServiceTest : public ::testing::Test {
 public:
  testing::StrictMock<EncryptionMock> encryptionMock_;
  testing::StrictMock<Http::HttpMock> HttpMock_;
  testing::StrictMock<::Time::TimeMock> timeMock_;

  BinanceService sut_{encryptionMock_, HttpMock_, timeMock_, dummyBinanceUrlPrefix};
};

TEST_F(BinanceServiceTest, WhenGetServerTimeCalled_ThenHttpGetIsInvoked) {
  const std::string binanceResponse = R"({"serverTime": 1234567890})";

  EXPECT_CALL(HttpMock_, get(dummyBinanceUrlPrefix.val_ + "/time", "")).WillOnce(Return(binanceResponse));
  EXPECT_EQ(Time{binanceResponse}, sut_.getServerTime());
}

TEST_F(BinanceServiceTest, WhenGetPriceCalled_ThenHttpGetIsInvoked) {
  const std::string binanceResponse = R"({"symbol": "BTCUSDT", "price": "50000"})";

  EXPECT_CALL(HttpMock_, get(dummyBinanceUrlPrefix.val_ + "/ticker/price?symbol=BTCUSDT", ""))
      .WillOnce(Return(binanceResponse));

  ApiGateway::Price outputPrice = sut_.getPrice(btcUsdtTradingPair);
  EXPECT_EQ(ApiGateway::Price{"50000"}, outputPrice);
}

TEST_F(BinanceServiceTest, WhenGetPricesCalled_ThenHttpGetIsInvoked) {
  const std::vector<ApiGateway::TradingPairSymbol> tradingPairSymbols{btcUsdtTradingPair, ethUsdtTradingPair};
  const std::string binanceResponse =
      R"([{"symbol": "BTCUSDT", "price": "50000"}, {"symbol": "ETHUSDT", "price": "4000"}])";

  EXPECT_CALL(HttpMock_, get(dummyBinanceUrlPrefix.val_ + "/ticker/price?symbols=[\"BTCUSDT\",\"ETHUSDT\"]", ""))
      .WillOnce(Return(binanceResponse));

  const AssetPrices outputPrices = sut_.getPrices(tradingPairSymbols);
  EXPECT_EQ(2, outputPrices.size());
  EXPECT_TRUE(std::ranges::is_permutation(
      outputPrices, std::vector{SingleAssetPrice{btcUsdtTradingPair, ApiGateway::Price{"50000"}},
                                SingleAssetPrice{ethUsdtTradingPair, ApiGateway::Price{"4000"}}}));
}

TEST_F(BinanceServiceTest, WhenGetKlinesCalled_ThenHttpGetIsInvoked) {
  // const ApiGateway::TradingPairSymbol symbol{"BTCUSDT"};
  // const auto interval = KlineInterval::OneHour;
  // const std::string klinesUrl = dummyBinanceUrlPrefix.val_ + "/klines?symbol=BTCUSDT&interval=1h&limit=1000";

  // const std::string binanceResponse = R"([
  //       [1622548800000, "35000.00", "36000.00", "34000.00", "35500.00", "1000.00", 1622552399999, "35500000.00", 100,
  //       "500.00", "3550000.00", "0"]
  //   ])";

  // EXPECT_CALL(HttpMock_, get(klinesUrl, "")).WillOnce(Return(binanceResponse));

  // KlineSequence outputSequence = sut_.getKlines(symbol, interval);
  // EXPECT_EQ(1, outputSequence.size());
  // const Kline expectedKline{
  //     .openTime = Time{"1622548800000"},
  //     .closeTime = Time{"1622552399999"},
  //     .openPrice = ApiGateway::Price{"35000.00"},
  //     .closePrice = ApiGateway::Price{"35500.00"},
  //     .lowPrice = ApiGateway::Price{"34000.00"},
  //     .highPrice = ApiGateway::Price{"36000.00"},
  // };
  // EXPECT_EQ(expectedKline, outputSequence.at(0));
}

TEST_F(BinanceServiceTest, WhenGetOwnedAssetsQuantityCalled_ThenHttpGetIsInvoked) {
  const std::string accountUrl = dummyBinanceUrlPrefix.val_ + "/account?timestamp=1234567890&signature=dummy_signature";
  const std::string binanceResponse = R"({"balances":[{"asset":"BTC","free":"0.1"},{"asset":"ETH","free":"1.0"}]})";

  EXPECT_CALL(timeMock_, getTimeSinceEpoch).WillOnce(Return(1234567890));
  EXPECT_CALL(encryptionMock_, generateSignature("timestamp=1234567890")).WillOnce(Return("dummy_signature"));
  EXPECT_CALL(encryptionMock_, getApiKey()).WillOnce(Return("dummy_api_key"));

  EXPECT_CALL(HttpMock_, get(accountUrl, "X-MBX-APIKEY: dummy_api_key")).WillOnce(Return(binanceResponse));

  ApiGateway::AssetQuantities outputAssets = sut_.getOwnedAssetsQuantity();
  EXPECT_EQ(2, outputAssets.size());
  EXPECT_TRUE(std::ranges::is_permutation(
      outputAssets,
      std::vector{ApiGateway::SingleAssetQuantity{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"0.1"},
                                                  ApiGateway::MarketId::Binance},
                  ApiGateway::SingleAssetQuantity{ApiGateway::AssetSymbol{"ETH"}, ApiGateway::AssetQuantity{"1.0"},
                                                  ApiGateway::MarketId::Binance}}));
}

TEST_F(BinanceServiceTest, WhengetOwnedAssetsQuantityAndValueCalled_ThenReturnsAssetsWithCalculatedUSDTValue) {
  const std::string accountUrl = dummyBinanceUrlPrefix.val_ + "/account?timestamp=1234567890&signature=dummy_signature";
  const std::string binanceResponse = R"({"balances":[{"asset":"BTC","free":"0.1"},{"asset":"ETH","free":"2.0"}]})";

  EXPECT_CALL(timeMock_, getTimeSinceEpoch).WillOnce(Return(1234567890));
  EXPECT_CALL(encryptionMock_, generateSignature("timestamp=1234567890")).WillOnce(Return("dummy_signature"));
  EXPECT_CALL(encryptionMock_, getApiKey()).WillOnce(Return("dummy_api_key"));

  EXPECT_CALL(HttpMock_, get(accountUrl, "X-MBX-APIKEY: dummy_api_key")).WillOnce(Return(binanceResponse));

  EXPECT_CALL(HttpMock_, get(tradingPairsUrl, "")).WillOnce(Return(binanceTradingPairsResponse));

  const std::string binanceGetPricesResponse =
      R"([{"symbol": "BTCUSDT", "price": "50000"}, {"symbol": "ETHUSDT", "price": "4000"}])";
  EXPECT_CALL(HttpMock_, get(dummyBinanceUrlPrefix.val_ + "/ticker/price?symbols=[\"BTCUSDT\",\"ETHUSDT\"]", ""))
      .WillOnce(Return(binanceGetPricesResponse));

  auto result = sut_.getOwnedAssetsQuantityAndValue();

  ASSERT_EQ(result.size(), 2);
  EXPECT_EQ(result[0].assetSymbol, ApiGateway::AssetSymbol{"BTC"});
  EXPECT_EQ(result[0].freeQuantity, ApiGateway::AssetQuantity{"0.1"});
  EXPECT_NEAR(std::stod(result[0].usdtValue.val_), 5000.0, 0.001);
  EXPECT_EQ(result[1].assetSymbol, ApiGateway::AssetSymbol{"ETH"});
  EXPECT_EQ(result[1].freeQuantity, ApiGateway::AssetQuantity{"2.0"});
  EXPECT_NEAR(std::stod(result[1].usdtValue.val_), 8000.0, 0.001);
}

TEST_F(BinanceServiceTest, WhenGetOwnedAssetValuesCalled_ThenReturnsAssetValuesWithCalculatedUSDTValue) {
  const ApiGateway::AssetSymbol quoteAsset{"USDT"};

  const std::string accountUrl = dummyBinanceUrlPrefix.val_ + "/account?timestamp=1234567890&signature=dummy_signature";
  const std::string binanceResponse = R"({"balances":[{"asset":"BTC","free":"0.1"},{"asset":"ETH","free":"2.0"}]})";

  EXPECT_CALL(timeMock_, getTimeSinceEpoch).WillOnce(Return(1234567890));
  EXPECT_CALL(encryptionMock_, generateSignature("timestamp=1234567890")).WillOnce(Return("dummy_signature"));
  EXPECT_CALL(encryptionMock_, getApiKey()).WillOnce(Return("dummy_api_key"));

  EXPECT_CALL(HttpMock_, get(accountUrl, "X-MBX-APIKEY: dummy_api_key")).WillOnce(Return(binanceResponse));

  EXPECT_CALL(HttpMock_, get(tradingPairsUrl, "")).WillOnce(Return(binanceTradingPairsResponse));

  const std::string binanceGetPricesResponse =
      R"([{"symbol": "BTCUSDT", "price": "50000"}, {"symbol": "ETHUSDT", "price": "4000"}])";
  EXPECT_CALL(HttpMock_, get(dummyBinanceUrlPrefix.val_ + "/ticker/price?symbols=[\"BTCUSDT\",\"ETHUSDT\"]", ""))
      .WillOnce(Return(binanceGetPricesResponse));

  auto result = sut_.getOwnedAssetValues(quoteAsset);

  ASSERT_EQ(result.size(), 2);
  EXPECT_EQ(result[0].baseSymbol, ApiGateway::AssetSymbol{"BTC"});
  EXPECT_EQ(result[0].quoteAsset, quoteAsset);
  EXPECT_NEAR(std::stod(result[0].value->val_), 5000.0, 0.001);
  EXPECT_EQ(result[1].baseSymbol, ApiGateway::AssetSymbol{"ETH"});
  EXPECT_EQ(result[1].quoteAsset, quoteAsset);
  EXPECT_NEAR(std::stod(result[1].value->val_), 8000.0, 0.001);
}

TEST_F(BinanceServiceTest, WhenMakeOrderCalled_ThenHttpPostIsInvoked) {
  const std::string queryString =
      "symbol=BTCUSDT&side=BUY&type=LIMIT&timeInForce=GTC&quantity=0.01&price=50000&recvWindow=5000&timestamp="
      "1234567890";
  const std::string orderUrl = dummyBinanceUrlPrefix.val_ + "/order?" + queryString + "&signature=dummy_signature";
  const std::string binanceResponse = R"({"orderId":123456,"status":"NEW"})";

  EXPECT_CALL(timeMock_, getTimeSinceEpoch).WillOnce(Return(1234567890));
  EXPECT_CALL(encryptionMock_, generateSignature(queryString)).WillOnce(Return("dummy_signature"));
  EXPECT_CALL(encryptionMock_, getApiKey()).WillOnce(Return("dummy_api_key"));

  EXPECT_CALL(HttpMock_, post(orderUrl, "X-MBX-APIKEY: dummy_api_key"))
      .WillOnce(Return(Http::Response{Http::HttpStatusCode{200}, Http::HttpBody{binanceResponse}}));

  ApiGateway::OrderResult result = sut_.makeOrder(btcUsdtTradingPair, ApiGateway::OrderSide::Buy,
                                                  ApiGateway::AssetQuantity{"0.01"}, ApiGateway::Price{"50000"});
  EXPECT_EQ(ApiGateway::OrderResult::Success, result);
}

TEST_F(BinanceServiceTest, WhenMakeMarketTypeOrderWithQuoteQuantityCalled_ThenHttpPostIsInvoked) {
  const std::string queryString =
      "symbol=BTCUSDT&side=BUY&type=MARKET&quoteOrderQty=1000&recvWindow=5000&timestamp=1234567890";
  const std::string orderUrl = dummyBinanceUrlPrefix.val_ + "/order?" + queryString + "&signature=dummy_signature";
  const std::string binanceResponse = R"({"orderId":123456,"status":"NEW"})";

  EXPECT_CALL(timeMock_, getTimeSinceEpoch).WillOnce(Return(1234567890));
  EXPECT_CALL(encryptionMock_, generateSignature(queryString)).WillOnce(Return("dummy_signature"));
  EXPECT_CALL(encryptionMock_, getApiKey()).WillOnce(Return("dummy_api_key"));

  EXPECT_CALL(HttpMock_, post(orderUrl, "X-MBX-APIKEY: dummy_api_key"))
      .WillOnce(Return(Http::Response{Http::HttpStatusCode{200}, Http::HttpBody{binanceResponse}}));

  ApiGateway::OrderResult result = sut_.makeMarketTypeOrderWithQuoteQuantity(
      btcUsdtTradingPair, ApiGateway::OrderSide::Buy, ApiGateway::AssetQuantity{"1000"});
  EXPECT_EQ(ApiGateway::OrderResult::Success, result);
}

TEST_F(BinanceServiceTest, WhenGetAllTradingPairsCalled_ThenHttpGetIsInvoked) {
  EXPECT_CALL(HttpMock_, get(tradingPairsUrl, "")).WillOnce(Return(binanceTradingPairsResponse));

  TradingPairs outputPairs = sut_.getAllTradingPairs();
  EXPECT_EQ(3, outputPairs.size());
  EXPECT_TRUE(std::ranges::is_permutation(outputPairs,
                                          TradingPairs{btcUsdtTradingPair, ethUsdtTradingPair, ethEurTradingPair}));
}

TEST_F(BinanceServiceTest, WhenGetTradingPairsWithQuoteAssetCalled_ThenHttpGetIsInvoked) {
  EXPECT_CALL(HttpMock_, get(tradingPairsUrl, "")).WillOnce(Return(binanceTradingPairsResponse));

  TradingPairs outputPairs = sut_.getTradingPairsWithQuoteAsset(ApiGateway::AssetSymbol{"USDT"});
  EXPECT_EQ(2, outputPairs.size());
  EXPECT_TRUE(std::ranges::is_permutation(outputPairs, TradingPairs{btcUsdtTradingPair, ethUsdtTradingPair}));
}

TEST_F(BinanceServiceTest, WhenGetTradingPairsWithBaseAssetCalled_ThenHttpGetIsInvoked) {
  EXPECT_CALL(HttpMock_, get(tradingPairsUrl, "")).WillOnce(Return(binanceTradingPairsResponse));

  TradingPairs outputPairs = sut_.getTradingPairsWithBaseAsset(ApiGateway::AssetSymbol{"ETH"});
  EXPECT_EQ(2, outputPairs.size());
  EXPECT_TRUE(std::ranges::is_permutation(outputPairs, TradingPairs{ethUsdtTradingPair, ethEurTradingPair}));
}

TEST_F(BinanceServiceTest, WhenGetOpenOrdersCalled_ThenHttpGetIsInvoked) {
  const std::string accountUrl =
      dummyBinanceUrlPrefix.val_ + "/openOrders?timestamp=1234567890&signature=dummy_signature";
  const std::string binanceResponse =
      R"([{"symbol":"BTCUSDT","orderId":123456,"price":"50000","origQty":"0.01","executedQty":"0.00","side":"BUY"},
                                          {"symbol":"ETHUSDT","orderId":654321,"price":"4000","origQty":"1.00",
                                           "executedQty":"0.50","side":"SELL"}])";

  EXPECT_CALL(timeMock_, getTimeSinceEpoch).WillOnce(Return(1234567890));
  EXPECT_CALL(encryptionMock_, generateSignature("timestamp=1234567890")).WillOnce(Return("dummy_signature"));
  EXPECT_CALL(encryptionMock_, getApiKey()).WillOnce(Return("dummy_api_key"));

  EXPECT_CALL(HttpMock_, get(accountUrl, "X-MBX-APIKEY: dummy_api_key")).WillOnce(Return(binanceResponse));

  ApiGateway::Orders outputOrders = sut_.getOpenOrders();
  EXPECT_EQ(2, outputOrders.size());
  EXPECT_TRUE(std::ranges::is_permutation(
      outputOrders,
      std::vector{ApiGateway::SingleOrder{btcUsdtTradingPair, ApiGateway::OrderId{123456}, ApiGateway::Price{"50000"},
                                          ApiGateway::AssetQuantity{"0.01"}, ApiGateway::AssetQuantity{"0.00"},
                                          ApiGateway::OrderSide::Buy},
                  ApiGateway::SingleOrder{ethUsdtTradingPair, ApiGateway::OrderId{654321}, ApiGateway::Price{"4000"},
                                          ApiGateway::AssetQuantity{"1.00"}, ApiGateway::AssetQuantity{"0.50"},
                                          ApiGateway::OrderSide::Sell}}));
}

TEST_F(BinanceServiceTest, WhenCancelAllOrdersOnASymbolCalled_ThenHttpDeleteIsInvoked) {
  const std::string queryString = "symbol=BTCUSDT&recvWindow=5000&timestamp=1234567890";
  const std::string orderUrl = dummyBinanceUrlPrefix.val_ + "/openOrders?" + queryString + "&signature=dummy_signature";
  const std::string binanceResponse = R"({"code":200,"msg":"Orders cancelled"})";

  EXPECT_CALL(timeMock_, getTimeSinceEpoch).WillOnce(Return(1234567890));
  EXPECT_CALL(encryptionMock_, generateSignature(queryString)).WillOnce(Return("dummy_signature"));
  EXPECT_CALL(encryptionMock_, getApiKey()).WillOnce(Return("dummy_api_key"));

  EXPECT_CALL(HttpMock_, del(orderUrl, "X-MBX-APIKEY: dummy_api_key"))
      .WillOnce(Return(Http::Response{Http::HttpStatusCode{200}, Http::HttpBody{binanceResponse}}));

  ApiGateway::OrderResult result = sut_.cancelAllOrdersOnASymbol(btcUsdtTradingPair);
  EXPECT_EQ(ApiGateway::OrderResult::Success, result);
}

}  // namespace MarketService::Binance
