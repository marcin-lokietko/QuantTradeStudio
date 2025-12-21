Feature: Moving Average Crossover Bot Backtesting


# Simulation since 1760723940000 (Fri Oct 17 2025 17:59:00 GMT+0000) to 1760724479000 (Fri Oct 17 2025 18:07:59 GMT+0000)
# Bot's executionPeriod is set to 61, so is executes once per each kline. First kline is ignored (bot does first iteration after 61s from start)
# A single sell order of ETH is done - see: "Ordering a trade when short and long averages cross over", example 2
Scenario: Backtesting of ordering a trade when short and long averages cross over
    Given MarketService mock is running
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    And MarketService mock expects invocations on GET /klines and will return responses as defined in file "klines/3DayAverageFallsBelow6DayAverage.json"
    And Backend is available
    When Request POST /testBot is sent with body "{"botName":"MovingAverageCrossover","executionPeriod":61,"shortTermMovingAverageLength":3,"longTermMovingAverageLength":6,"quoteAsset":"USDT","baseAssets":["BTC", "ETH"],"transactionFeePercent":1,"initialOwnedAssets":[{"assetSymbol":"ETH","freeQuantity":"10"},{"assetSymbol":"BTC","freeQuantity":"1"},{"assetSymbol":"USDT","freeQuantity":"10000"}],"simulationStart":1760723940000,"simulationEnd":1760724479000}"
    Then Response for POST /testBot was received with status code "200" and body
"""
{
  "absoluteAsset": "USDT",
  "assetHistoryIfHeld": [
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20100.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760723940000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20100.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724000000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20100.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724060000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20100.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724120000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20200.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724180000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20200.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724240000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20200.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724300000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "19500.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724360000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "19000.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724420000
    }
  ],
  "botAssetHistory": [
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20100.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760723940000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20100.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724000000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20100.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724060000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20100.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724120000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20200.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724180000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20200.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724240000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20200.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724300000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "19500.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724360000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "0.000000",
          "usdtValue": "0.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "80000.000000"
        },
        {
          "assetSymbol": "USDT",
          "freeQuantity": "29305.000000",
          "usdtValue": "29305.000000"
        }
      ],
      "timestamp": 1760724420000
    }
  ],
  "totalProfitOrLossInAbsolute": "-795.000000",
  "totalProfitOrLossInAbsoluteIfHeld": "-1100.000000",
  "totalProfitOrLossInPercent": -0.7220708446866485,
  "totalProfitOrLossInPercentIfHeld": -0.9990917347865577
}
"""