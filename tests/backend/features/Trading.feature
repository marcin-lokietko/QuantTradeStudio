Feature: Trading


Scenario: Invoking /assets returns owned assets
    Given Backend is available
    And MarketService mock is running with default configuration
    When Request GET /assets is sent
    Then Response for GET /assets was received with status code "200" and body "[{"assetSymbol":"BTC","freeQuantity":"1.234","usdtValue":"98720.000000"},{"assetSymbol":"ETH","freeQuantity":"123.4","usdtValue":"246800.000000"}]"


Scenario Outline: Invoking /openOrders returns active orders
    Given Backend is available
    And MarketService mock is running
    And MarketService mock expects invocations on GET /openOrders and will return "<market_service_open_orders>"
    When Request GET /openOrders is sent
    Then Response for GET /openOrders was received with status code "200" and body "<algo_trader_open_orders>"
    Examples:
      | market_service_open_orders                                                                                                                                                                         | algo_trader_open_orders                                                                                                                                                                                                                |
      | []                                                                                                                                                                                                 | []                                                                                                                                                                                                                                     |
      | [{"symbol":"BTCUSDT","orderId":1,"price":"80000","origQty":"0.5","executedQty":"0.25","side":"BUY"}]                                                                                               | [{"assetPair":"BTCUSDT","executedQuantity":"0.25","orderId":1,"orderSide":"Buy","origQuantity":"0.5","price":"80000"}]                                                                                                                 |
      | [{"symbol":"BTCUSDT","orderId":1,"price":"80000","origQty":"0.5","executedQty":"0.25","side":"BUY"},{"symbol":"ETHUSDT","orderId":2,"price":"2000","origQty":"2","executedQty":"1","side":"SELL"}] | [{"assetPair":"BTCUSDT","executedQuantity":"0.25","orderId":1,"orderSide":"Buy","origQuantity":"0.5","price":"80000"},{"assetPair":"ETHUSDT","executedQuantity":"1","orderId":2,"orderSide":"Sell","origQuantity":"2","price":"2000"}] |


Scenario Outline: Invoking /makeOrder places order via market service
    Given Backend is available
    And MarketService mock is running with default configuration
    When Request POST /makeOrder is sent with body "{"selectedBaseAsset":"<sent_base_asset>","selectedQuoteAsset":"<sent_quote_asset>","orderSide":"<sent_side>","baseAssetAmount":"<amount>"}"
    Then MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"<expected_trading_pair>","side":"<expected_side>","type":"LIMIT","timeInForce":"GTC","quantity":"<amount>","price":"","recvWindow":"5000"}"
    Examples:
      | sent_base_asset     | sent_quote_asset     | sent_side     | expected_trading_pair | expected_side | amount  |
      | ETH                 | BTC                  | Buy           | ETHBTC                | BUY           | 0.1     |
      | ETH                 | BTC                  | Sell          | ETHBTC                | SELL          | 0.2     |
      | ETH                 | USDT                 | Buy           | ETHUSDT               | BUY           | 11      |
      | ETH                 | USDT                 | Sell          | ETHUSDT               | SELL          | 22      |
      | BTC                 | USDT                 | Buy           | BTCUSDT               | BUY           | 222.222 |
      | BTC                 | USDT                 | Sell          | BTCUSDT               | SELL          | 333.333 |
