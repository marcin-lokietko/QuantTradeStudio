#include "MarketService/Binance/BinanceService.hpp"

#include <ranges>

#include "GeneratedMocks/EncryptionMock.hpp"
#include "GeneratedMocks/HttpMock.hpp"
#include "GeneratedMocks/TimeMock.hpp"
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
  BinanceServiceTest() {
    EXPECT_CALL(HttpMock_, get(tradingPairsUrl, "")).WillRepeatedly(Return(binanceTradingPairsResponse));
  }

  testing::StrictMock<EncryptionMock> encryptionMock_;
  testing::StrictMock<Http::HttpMock> HttpMock_;
  testing::StrictMock<::Time::TimeMock> timeMock_;

  BinanceService MakeSut() { return BinanceService{encryptionMock_, HttpMock_, timeMock_, dummyBinanceUrlPrefix}; }
};

TEST_F(BinanceServiceTest, WhenGetServerTimeCalled_ThenHttpGetIsInvoked) {
  const std::string binanceResponse = R"({"serverTime": 1234567890})";

  EXPECT_CALL(HttpMock_, get(dummyBinanceUrlPrefix.val_ + "/time", "")).WillOnce(Return(binanceResponse));
  EXPECT_EQ(Time{binanceResponse}, MakeSut().getServerTime());
}

TEST_F(BinanceServiceTest, WhenGetPriceCalled_ThenHttpGetIsInvoked) {
  const std::string binanceResponse = R"({"symbol": "BTCUSDT", "price": "50000"})";

  EXPECT_CALL(HttpMock_, get(dummyBinanceUrlPrefix.val_ + "/ticker/price?symbol=BTCUSDT", ""))
      .WillOnce(Return(binanceResponse));

  ApiGateway::Price outputPrice = MakeSut().getPrice(btcUsdtTradingPair);
  EXPECT_EQ(ApiGateway::Price{"50000"}, outputPrice);
}

TEST_F(BinanceServiceTest, WhenGetPricesCalled_ThenHttpGetIsInvoked) {
  const std::vector<ApiGateway::TradingPairSymbol> tradingPairSymbols{btcUsdtTradingPair, ethUsdtTradingPair};
  const std::string binanceResponse =
      R"([{"symbol": "BTCUSDT", "price": "50000"}, {"symbol": "ETHUSDT", "price": "4000"}])";

  EXPECT_CALL(HttpMock_, get(dummyBinanceUrlPrefix.val_ + "/ticker/price?symbols=[\"BTCUSDT\",\"ETHUSDT\"]", ""))
      .WillOnce(Return(binanceResponse));

  const AssetPrices outputPrices = MakeSut().getPrices(tradingPairSymbols);
  EXPECT_EQ(2, outputPrices.size());
  EXPECT_TRUE(std::ranges::is_permutation(
      outputPrices, std::vector{SingleAssetPrice{btcUsdtTradingPair, ApiGateway::Price{"50000"}},
                                SingleAssetPrice{ethUsdtTradingPair, ApiGateway::Price{"4000"}}}));
}

TEST_F(BinanceServiceTest, WhenGetKlinesCalled_ThenHttpGetIsInvoked) {
  const ApiGateway::TradingPairSymbol symbol{AssetSymbol{"BTC"}, AssetSymbol{"USDT"}};
  const auto interval = KlineInterval::OneHour;
  const std::string klinesUrl =
      dummyBinanceUrlPrefix.val_ +
      "/klines?symbol=BTCUSDT&interval=1h&startTime=1622545200000&endTime=1622555999999&limit=1000";

  const std::string binanceResponse = R"([
        [1622548800000, "35000.00", "36000.00", "34000.00", "35500.00", "1000.00", 1622552399999, "35500000.00", 100,
        "500.00", "3550000.00", "0"]
    ])";

  EXPECT_CALL(HttpMock_, get(klinesUrl, "")).WillOnce(Return(binanceResponse));

  const std::chrono::system_clock::time_point startTime{std::chrono::milliseconds{1622548800000}};
  const std::chrono::system_clock::time_point endTime{std::chrono::milliseconds{1622552399999}};

  KlineSequence outputSequence = MakeSut().getKlines(symbol, interval, startTime, endTime);
  EXPECT_EQ(1, outputSequence.size());
  const Kline expectedKline{
      .openTime = startTime,
      .closeTime = endTime,
      .openPrice = ApiGateway::Price{"35000.00"},
      .closePrice = ApiGateway::Price{"35500.00"},
      .lowPrice = ApiGateway::Price{"34000.00"},
      .highPrice = ApiGateway::Price{"36000.00"},
  };
  EXPECT_EQ(expectedKline, outputSequence.at(0));
}

TEST_F(BinanceServiceTest,
       WhenGetKlinesCalledWithLongTimeRange_ThenHttpGetIsInvoked2TimesAndResultsAreCombinedWithoutDuplicates) {
  const ApiGateway::TradingPairSymbol symbol{AssetSymbol{"BTC"}, AssetSymbol{"USDT"}};
  const auto interval = KlineInterval::OneMinute;

  const std::chrono::system_clock::time_point startTime{std::chrono::milliseconds{1622548800000}};
  const std::chrono::system_clock::time_point endTime = startTime + std::chrono::minutes{1500};

  const uint64_t startTimeMs =
      std::chrono::duration_cast<std::chrono::milliseconds>(startTime.time_since_epoch()).count();
  // paging is determined based on end time of last kline in received batch, so it has to make sense in the test:
  const uint64_t splitTimeMs = startTimeMs + std::chrono::minutes{995}.count() * 60 * 1000;
  const uint64_t endTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count();

  // in reality, the first response would have about 1k elements in case of paging, but for the sake of the test this
  // would be unnecessary
  const std::string binanceResponse1 = std::format(
      R"([
        [1622548800000, "35000.00", "36000.00", "34000.00", "35500.00", "1000.00", 1622548859000, "35500000.00", 100,
        "500.00", "3550000.00", "0"],
        [1622548860000, "35001.00", "36001.00", "34001.00", "35501.00", "1000.00", 1622548919000, "35500000.00", 100,
        "500.00", "3550000.00", "0"],
        [1622548920000, "35002.00", "36002.00", "34002.00", "35502.00", "1000.00", {}, "35500000.00", 100,
        "500.00", "3550000.00", "0"]
    ])",
      splitTimeMs);
  const std::string binanceResponse2 = std::format(
      R"([
        [1622548920000, "35002.00", "36002.00", "34002.00", "35502.00", "1000.00", {}, "35500000.00", 100,
        "500.00", "3550000.00", "0"],
        [1622548980000, "35003.00", "36003.00", "34003.00", "35503.00", "1000.00", {}, "35500000.00", 100,
        "500.00", "3550000.00", "0"]
    ])",
      splitTimeMs, endTimeMs);

  const uint64_t oneKlineDurationMs =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::minutes{1}).count();
  const std::string klinesUrl1 =
      dummyBinanceUrlPrefix.val_ + std::format("/klines?symbol=BTCUSDT&interval=1m&startTime={}&endTime={}&limit=1000",
                                               startTimeMs - oneKlineDurationMs, splitTimeMs + oneKlineDurationMs);
  const std::string klinesUrl2 =
      dummyBinanceUrlPrefix.val_ + std::format("/klines?symbol=BTCUSDT&interval=1m&startTime={}&endTime={}&limit=1000",
                                               splitTimeMs + 1 - oneKlineDurationMs, endTimeMs + oneKlineDurationMs);

  EXPECT_CALL(HttpMock_, get(klinesUrl1, "")).WillOnce(Return(binanceResponse1));
  EXPECT_CALL(HttpMock_, get(klinesUrl2, "")).WillOnce(Return(binanceResponse2));

  const KlineSequence outputSequence = MakeSut().getKlines(symbol, interval, startTime, endTime);

  EXPECT_EQ(4, outputSequence.size());

  const Kline expectedKline1{
      .openTime = startTime,
      .closeTime = startTime + std::chrono::seconds{59},
      .openPrice = ApiGateway::Price{"35000.00"},
      .closePrice = ApiGateway::Price{"35500.00"},
      .lowPrice = ApiGateway::Price{"34000.00"},
      .highPrice = ApiGateway::Price{"36000.00"},
  };
  EXPECT_EQ(expectedKline1, outputSequence.at(0));

  const Kline expectedKline2{
      .openTime = startTime + std::chrono::minutes{1},
      .closeTime = startTime + std::chrono::minutes{1} + std::chrono::seconds{59},
      .openPrice = ApiGateway::Price{"35001.00"},
      .closePrice = ApiGateway::Price{"35501.00"},
      .lowPrice = ApiGateway::Price{"34001.00"},
      .highPrice = ApiGateway::Price{"36001.00"},
  };
  EXPECT_EQ(expectedKline2, outputSequence.at(1));

  const Kline expectedKline3{
      .openTime = startTime + std::chrono::minutes{2},
      .closeTime = std::chrono::system_clock::time_point{std::chrono::milliseconds{splitTimeMs}},
      .openPrice = ApiGateway::Price{"35002.00"},
      .closePrice = ApiGateway::Price{"35502.00"},
      .lowPrice = ApiGateway::Price{"34002.00"},
      .highPrice = ApiGateway::Price{"36002.00"},
  };
  EXPECT_EQ(expectedKline3, outputSequence.at(2));

  const Kline expectedKline4{
      .openTime = startTime + std::chrono::minutes{3},
      .closeTime = std::chrono::system_clock::time_point{std::chrono::milliseconds{endTimeMs}},
      .openPrice = ApiGateway::Price{"35003.00"},
      .closePrice = ApiGateway::Price{"35503.00"},
      .lowPrice = ApiGateway::Price{"34003.00"},
      .highPrice = ApiGateway::Price{"36003.00"},
  };
  EXPECT_EQ(expectedKline4, outputSequence.at(3));
}

TEST_F(BinanceServiceTest, WhenGetOwnedAssetsQuantityCalled_ThenHttpGetIsInvoked) {
  const std::string accountUrl = dummyBinanceUrlPrefix.val_ + "/account?timestamp=1234567890&signature=dummy_signature";
  const std::string binanceResponse = R"({"balances":[{"asset":"BTC","free":"0.1"},{"asset":"ETH","free":"1.0"}]})";

  EXPECT_CALL(timeMock_, getTimeSinceEpoch).WillOnce(Return(1234567890));
  EXPECT_CALL(encryptionMock_, generateSignature("timestamp=1234567890")).WillOnce(Return("dummy_signature"));
  EXPECT_CALL(encryptionMock_, getApiKey()).WillOnce(Return("dummy_api_key"));

  EXPECT_CALL(HttpMock_, get(accountUrl, "X-MBX-APIKEY: dummy_api_key")).WillOnce(Return(binanceResponse));

  ApiGateway::AssetQuantities outputAssets = MakeSut().getOwnedAssetsQuantity();
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

  const std::string binanceGetPricesResponse =
      R"([{"symbol": "BTCUSDT", "price": "50000"}, {"symbol": "ETHUSDT", "price": "4000"}])";
  EXPECT_CALL(HttpMock_, get(dummyBinanceUrlPrefix.val_ + "/ticker/price?symbols=[\"BTCUSDT\",\"ETHUSDT\"]", ""))
      .WillOnce(Return(binanceGetPricesResponse));

  auto result = MakeSut().getOwnedAssetsQuantityAndValue();

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

  const std::string binanceGetPricesResponse =
      R"([{"symbol": "BTCUSDT", "price": "50000"}, {"symbol": "ETHUSDT", "price": "4000"}])";
  EXPECT_CALL(HttpMock_, get(dummyBinanceUrlPrefix.val_ + "/ticker/price?symbols=[\"BTCUSDT\",\"ETHUSDT\"]", ""))
      .WillOnce(Return(binanceGetPricesResponse));

  auto result = MakeSut().getOwnedAssetValues(quoteAsset);

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

  ApiGateway::OrderResult result = MakeSut().makeOrder(btcUsdtTradingPair, ApiGateway::OrderSide::Buy,
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

  ApiGateway::OrderResult result = MakeSut().makeMarketTypeOrderWithQuoteQuantity(
      btcUsdtTradingPair, ApiGateway::OrderSide::Buy, ApiGateway::AssetQuantity{"1000"});
  EXPECT_EQ(ApiGateway::OrderResult::Success, result);
}

TEST_F(BinanceServiceTest, WhenGetAllTradingPairsCalled_ThenHttpGetIsInvoked) {
  TradingPairs outputPairs = MakeSut().getAllTradingPairs();
  EXPECT_EQ(3, outputPairs.size());
  EXPECT_TRUE(std::ranges::is_permutation(outputPairs,
                                          TradingPairs{btcUsdtTradingPair, ethUsdtTradingPair, ethEurTradingPair}));
}

TEST_F(BinanceServiceTest, WhenGetTradingPairsWithQuoteAssetCalled_ThenHttpGetIsInvoked) {
  TradingPairs outputPairs = MakeSut().getTradingPairsWithQuoteAsset(ApiGateway::AssetSymbol{"USDT"});
  EXPECT_EQ(2, outputPairs.size());
  EXPECT_TRUE(std::ranges::is_permutation(outputPairs, TradingPairs{btcUsdtTradingPair, ethUsdtTradingPair}));
}

TEST_F(BinanceServiceTest, WhenGetTradingPairsWithBaseAssetCalled_ThenHttpGetIsInvoked) {
  TradingPairs outputPairs = MakeSut().getTradingPairsWithBaseAsset(ApiGateway::AssetSymbol{"ETH"});
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

  ApiGateway::Orders outputOrders = MakeSut().getOpenOrders();
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

  ApiGateway::OrderResult result = MakeSut().cancelAllOrdersOnASymbol(btcUsdtTradingPair);
  EXPECT_EQ(ApiGateway::OrderResult::Success, result);
}

}  // namespace MarketService::Binance
