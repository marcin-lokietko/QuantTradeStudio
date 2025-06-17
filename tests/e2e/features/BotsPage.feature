Feature: Bots page


# See tests/backend/features/Bots.feature, scenario "Single rebalancing of 2 assets" for explanation of the expected requests to MarketService
Scenario: Rebalancer bot is started
    Given AlgoTrader is running
    And MarketService mock is running with default configuration
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    When Bots page is opened
    And Bot "rebalancer" is selected
    And Rebalancer bot configuration is filled with execution period "10", quote asset "USDT", base assets ["BTC", "ETH"] with shares ["90", "10"] and confirmed
    And System runs for 5 sec
    Then MarketService method DELETE of endpoint /openOrders has been invoked with query params "{"symbol":"BTCUSDT"}"
    And MarketService method DELETE of endpoint /openOrders has been invoked with query params "{"symbol":"ETHUSDT"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"ETHUSDT", "side":"SELL", "type":"MARKET", "quoteOrderQty":"212248.000000"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"BTCUSDT", "side":"BUY", "type":"MARKET", "quoteOrderQty":"212248.000000"}"
