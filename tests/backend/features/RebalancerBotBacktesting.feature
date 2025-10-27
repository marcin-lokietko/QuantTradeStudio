Feature: Rebalancer Bot Backtesting

# Simulation since 1760724000000 (Fri Oct 17 2025 18:00:00 GMT+0000) to 1760724120000 (Fri Oct 17 2025 18:02:00 GMT+0000)
# The value of BTCUSDT and ETHUSDT (in klines) is constant in time and equal to 80000 and 2000.
# This means that only two trades are supposed to be done - see scenario "Single rebalancing of 2 assets" in feature "Rebalancer Bot" - the expected change of assets:
# ETH: 246 800 USDT -> 34 552 USDT (selling 212 248 USDT worth of ETH). After the transaction there should be: 10 000 + (212 248 * 0,99) = 220 125.52 USDT
# BTC: 98 720 USDT -> 310 968 USDT (buying 212 248 USDT worth of BTC, which is 212 248/80 000 = 2.6531 BTC). After the transaction there should be: 1.234 + (2.6531 * 0,99) = 3.860569 BTC
Scenario: Backtesting single rebalancing of 2 assets
    Given MarketService mock is running with default configuration
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    And MarketService mock expects invocations on GET /klines and will return responses as defined in file "klines/1.json"
    And Backend is available
    When Request POST /testBot is sent with body "{"botName":"Rebalancer","executionPeriod":60,"isExecutedImmediately":true,"quoteAsset":"USDT","baseAssetShares":[{"assetSymbol":"ETH","expectedShare":10},{"assetSymbol":"BTC","expectedShare":90}],"transactionFeePercent":1,"initialOwnedAssets":[{"assetSymbol":"ETH","freeQuantity":"123.4"},{"assetSymbol":"BTC","freeQuantity":"1.234"},{"assetSymbol":"USDT","freeQuantity":"10000"}],"simulationStart":1760724000000,"simulationEnd":1760724120000}"
    Then Response for POST /testBot was received with status code "200" and body
"""
{
  "absoluteAsset": "USDT",
  "assetHistoryIfHeld": [
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "123.4",
          "usdtValue": "246800.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1.234",
          "usdtValue": "98720.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724000000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "123.4",
          "usdtValue": "246800.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1.234",
          "usdtValue": "98720.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724060000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "123.4",
          "usdtValue": "246800.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1.234",
          "usdtValue": "98720.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724120000
    }
  ],
  "botAssetHistory": [
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "123.4",
          "usdtValue": "246800.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1.234",
          "usdtValue": "98720.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724000000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "17.276000",
          "usdtValue": "34552.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1.234",
          "usdtValue": "98720.000000"
        },
        {
          "assetSymbol": "USDT",
          "freeQuantity": "220125.520000",
          "usdtValue": "220125.520000"
        }
      ],
      "timestamp": 1760724060000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "17.276000",
          "usdtValue": "34552.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "3.860569",
          "usdtValue": "308845.520000"
        },
        {
          "assetSymbol": "USDT",
          "freeQuantity": "7877.520000",
          "usdtValue": "7877.520000"
        }
      ],
      "timestamp": 1760724120000
    }
  ],
  "totalProfitOrLossInAbsolute": "-4244.960000",
  "totalProfitOrLossInAbsoluteIfHeld": "0.000000",
  "totalProfitOrLossInPercent": -1.194014401440144,
  "totalProfitOrLossInPercentIfHeld": 0.0
}
"""
