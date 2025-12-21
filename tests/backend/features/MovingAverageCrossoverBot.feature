Feature: Moving Average Crossover Bot


### Example 1: price/3DayAverageRisesAbove6DayAverage.json - for BTCUSDT:
# after 6th price check: short term is 80010, long term is 80015     -> short term BELOW long term
# after 7th price check: short term is 80030, long term is 80023.(3) -> short term ABOVE long term
### Example 2: price/3DayAverageFallsBelow6DayAverage.json - for ETHUSDT:
# after 6th price check: short term is 2020, long term is 2015     -> short term ABOVE long term
# after 7th price check: short term is 1996.(6), long term is 2005 -> short term BELOW long term
Scenario Outline: Ordering a trade when short and long averages cross over
    Given MarketService mock is running
    And MarketService mock expects invocations on GET /openOrders and will return "[{"symbol":"BTCUSDT","orderId":2,"price":"80000","origQty":"0.01","executedQty":"0","side":"SELL"}]"
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    And MarketService mock expects invocations on POST /order and will return empty response
    And MarketService mock expects invocations on GET /ticker/price and will return responses as defined in file "<prices_file>"
    And MarketService mock expects invocations on GET /exchangeInfo and will return "{"symbols":[{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"},{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"}]}"
    And MarketService mock expects invocations on GET /account and will return "{"balances":[{"free":"1","asset":"BTC"},{"free":"50","asset":"ETH"},{"free":"10000","asset":"USDT"}]}"
    And Backend is available
    When Request POST /startBot is sent with body "{"botName":"MovingAverageCrossover","executionPeriod":1,"shortTermMovingAverageLength":3,"longTermMovingAverageLength":6,"quoteAsset":"USDT","baseAssets":["BTC", "ETH"]}"
    Then Response for POST /startBot was received with status code "200" and no body
    And System runs for 7.5 sec
    And MarketService method DELETE of endpoint /openOrders has been invoked with query params "{"symbol":"BTCUSDT"}"
    And MarketService method POST of endpoint /order has been invoked with query params "<expected_order_query_params>"
    Examples:
      | prices_file                                 | expected_order_query_params                                                  |
      | price/3DayAverageRisesAbove6DayAverage.json | {"symbol":"BTCUSDT", "side":"BUY", "type":"MARKET", "quoteOrderQty":"10000"} |
      | price/3DayAverageFallsBelow6DayAverage.json | {"symbol":"ETHUSDT", "side":"SELL", "type":"MARKET", "quantity":"50"}        |
