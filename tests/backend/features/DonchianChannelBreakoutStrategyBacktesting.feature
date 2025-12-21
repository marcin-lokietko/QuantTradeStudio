Feature: Donchian Channel Breakout Strategy Bot Backtesting

# Similar to "Ordering a trade when current close price is more extreme than exit/entry channel extremum" - Example 1 (buy signal is triggered for BTC)
# Klines are simulated from 1760724000000 (Fri Oct 17 2025 18:00:00 GMT+0000) to 1760724539000 (Fri Oct 17 2025 18:08:59 GMT+0000)
# Simulation since 1760724420000 (Fri Oct 17 2025 18:07:00 GMT+0000) to 1760724421000 (Fri Oct 17 2025 18:07:01 GMT+0000)
Scenario: Backtesting of ordering a sell when current close price is greater than entry channel maximum
    Given MarketService mock is running
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    And MarketService mock expects invocations on GET /klines and will return responses as defined in file "klines/currentClosePriceIsHigherThanEntryChannelMaximum2.json"
    And Backend is available
    When Request POST /testBot is sent with body "{"botName":"DonchianChannelBreakoutStrategy","executionInterval":"OneMinute","exitChannelLength":3,"entryChannelLength":6,"quoteAsset":"USDT","baseAssets":["BTC","ETH"],"transactionFeePercent":1,"initialOwnedAssets":[{"assetSymbol":"ETH","freeQuantity":"10"},{"assetSymbol":"BTC","freeQuantity":"1"},{"assetSymbol":"USDT","freeQuantity":"10000"}],"simulationStart":1760724420000,"simulationEnd":1760724421000}"
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
          "usdtValue": "20000.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "83000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724420000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20000.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "85000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724480000
    }
  ],
  "botAssetHistory": [
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20000.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "83000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724420000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "10",
          "usdtValue": "20000.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1.119277",
          "usdtValue": "95138.545000"
        },
        {
          "assetSymbol": "USDT",
          "freeQuantity": "0.000000",
          "usdtValue": "0.000000"
        }
      ],
      "timestamp": 1760724480000
    }
  ],
  "totalProfitOrLossInAbsolute": "2138.545000",
  "totalProfitOrLossInAbsoluteIfHeld": "2000.000000",
  "totalProfitOrLossInPercent": 1.8925176991150445,
  "totalProfitOrLossInPercentIfHeld": 1.7699115044247788
}
"""
