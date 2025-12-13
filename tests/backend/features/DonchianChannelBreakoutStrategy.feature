Feature: Donchian Channel Breakout Strategy Bot


# Example 1: Lowest kline low of last 3 klines (excluding the most recent kline) is 79000. Most recent kline close is 75000, so the sell signal is triggered
# Example 2: Highest kline high of last 6 klines (excluding the most recent kline) is 82000. Most recent kline close is 83000, so the buy signal is triggered
Scenario: Buying when current close price is higher than entry channel maximum
Scenario Outline: Ordering a trade when current close price is more extreme than exit/entry channel extremum
    Given MarketService mock is running
    And MarketService mock expects invocations on GET /openOrders and will return "[{"symbol":"BTCUSDT","orderId":2,"price":"80000","origQty":"0.01","executedQty":"0","side":"SELL"}]"
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    And MarketService mock expects invocations on POST /order and will return empty response
    And MarketService mock expects invocations on GET /klines and will return responses as defined in file "<klines_file>"
    And MarketService mock expects invocations on GET /exchangeInfo and will return "{"symbols":[{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"},{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"}]}"
    And MarketService mock expects invocations on GET /account and will return "{"balances":[{"free":"1","asset":"BTC"},{"free":"50","asset":"ETH"},{"free":"10000","asset":"USDT"}]}"
    And Backend is available
    When Request POST /startBot is sent with body "{"botName":"DonchianChannelBreakoutStrategy","executionInterval":"OneHour","exitChannelLength":3,"entryChannelLength":6,"quoteAsset":"USDT","baseAssets":["ETH","BTC"]}"
    Then Response for POST /startBot was received with status code "200" and no body
    And System runs for 1 sec
    And MarketService method DELETE of endpoint /openOrders has been invoked with query params "{"symbol":"BTCUSDT"}"
    And MarketService method POST of endpoint /order has been invoked with query params "<expected_order_query_params>"
    Examples:
      | klines_file                                                  | expected_order_query_params                                                  |
      | klines/currentClosePriceIsHigherThanEntryChannelMaximum.json | {"symbol":"BTCUSDT", "side":"BUY", "type":"MARKET", "quoteOrderQty":"10000"} |
      | klines/currentClosePriceIsLowerThanExitChannelMinimum.json   | {"symbol":"BTCUSDT", "side":"SELL", "type":"MARKET", "quantity":"1"}         |
