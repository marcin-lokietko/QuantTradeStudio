Feature: Rebalancer Bot Backtesting

# Note that backtester has limitation of one trade being simulated per single 1 minute kline


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


# Simulation since 1760724000000 (Fri Oct 17 2025 18:00:00 GMT+0000) to 1760724240000 (Fri Oct 17 2025 18:04:00 GMT+0000)
# The value of BTCUSDT and ETHUSDT (in klines) changes in time
# Two trades are supposed to be done - see scenario "Two rebalances of 2 assets" in feature "Rebalancer Bot"
# Initially user owns:
#  1. 50 ETH,   worth 100 000 USDT
#  2. 1 BTC,    worth 80 000 USDT
#  3. 10000 USDT
# The expected wallet after 1st rebalance is:
#  1. 53.96 ETH (bought 8000 USDT; 4*0.99=3.96ETH),   worth 53 960 USDT
#  2. 0.9 BTC (sold 8000 USDT),                       worth 90 000 USDT
#  3. 9920 USDT (10000 + 8000*0.99 - 8000)
#  Total ETH+BTC worth: 143 960 USDT. Proportions require 86 376 USDT (ETH) + 57 584 USDT (BTC). Difference is 90 000-57 584 = 32 416 USDT
# The expected wallet after 2nd rebalance is:
#  1. 86.05184 ETH,   (53.96 + 32 416*0,99/1000)      worth 100 000  USDT
#  2. 1 BTC,                                          worth 57 584 USDT
#  3. 9595.84 USDT (9 920 + 32 416*0,99 - 32 416)
Scenario: Backtesting two rebalances of 2 assets
    Given MarketService mock is running with default configuration
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    And MarketService mock expects invocations on GET /klines and will return responses as defined in file "klines/2.json"
    And Backend is available
    When Request POST /testBot is sent with body "{"botName":"Rebalancer","executionPeriod":60,"isExecutedImmediately":true,"quoteAsset":"USDT","baseAssetShares":[{"assetSymbol":"ETH","expectedShare":60},{"assetSymbol":"BTC","expectedShare":40}],"transactionFeePercent":1,"initialOwnedAssets":[{"assetSymbol":"ETH","freeQuantity":"50"},{"assetSymbol":"BTC","freeQuantity":"1"},{"assetSymbol":"USDT","freeQuantity":"10000"}],"simulationStart":1760724000000,"simulationEnd":1760724240000}"
    Then Response for POST /testBot was received with status code "200" and body
"""
{
  "absoluteAsset": "USDT",
  "assetHistoryIfHeld": [
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "50",
          "usdtValue": "100000.000000"
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
          "freeQuantity": "50",
          "usdtValue": "50000.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "100000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724060000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "50",
          "usdtValue": "50000.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "100000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724120000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "50",
          "usdtValue": "50000.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "100000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724180000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "50",
          "usdtValue": "50000.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "1",
          "usdtValue": "100000.000000"
        },
        { "assetSymbol": "USDT", "freeQuantity": "10000", "usdtValue": "10000" }
      ],
      "timestamp": 1760724240000
    }
  ],
  "botAssetHistory": [
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "50",
          "usdtValue": "100000.000000"
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
          "freeQuantity": "50",
          "usdtValue": "50000.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "0.900000",
          "usdtValue": "90000.000000"
        },
        {
          "assetSymbol": "USDT",
          "freeQuantity": "17920.000000",
          "usdtValue": "17920.000000"
        }
      ],
      "timestamp": 1760724060000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "53.960000",
          "usdtValue": "53960.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "0.900000",
          "usdtValue": "90000.000000"
        },
        {
          "assetSymbol": "USDT",
          "freeQuantity": "9920.000000",
          "usdtValue": "9920.000000"
        }
      ],
      "timestamp": 1760724120000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "53.960000",
          "usdtValue": "53960.000000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "0.575840",
          "usdtValue": "57584.000000"
        },
        {
          "assetSymbol": "USDT",
          "freeQuantity": "42011.840000",
          "usdtValue": "42011.840000"
        }
      ],
      "timestamp": 1760724180000
    },
    {
      "assets": [
        {
          "assetSymbol": "ETH",
          "freeQuantity": "86.051840",
          "usdtValue": "86051.840000"
        },
        {
          "assetSymbol": "BTC",
          "freeQuantity": "0.575840",
          "usdtValue": "57584.000000"
        },
        {
          "assetSymbol": "USDT",
          "freeQuantity": "9595.840000",
          "usdtValue": "9595.840000"
        }
      ],
      "timestamp": 1760724240000
    }
  ],
  "totalProfitOrLossInAbsolute": "-36768.320000",
  "totalProfitOrLossInAbsoluteIfHeld": "-30000.000000",
  "totalProfitOrLossInPercent": -19.35174736842105,
  "totalProfitOrLossInPercentIfHeld": -15.789473684210526
}
"""
