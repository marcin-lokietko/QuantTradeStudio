Feature: Bots page

# With the default configuration of MarketService, the user owns
#  1. 123.4 ETH, which is worth 123.4*2000=246 800 USDT
#  2. 1.234 BTC, which is worth 1.234*80000=98 720 USDT
# So the total worth of the owned ETH and BTC is 345 520
# With the expected value proportion of BTC/ETH= 9/1, the expected wallet is:
#  1. 34 552 USDT in ETH => need to sell ETH worth 246 800 - 34 552 = 212248 USDT
#  2. need to buy BTC worth 212248 USDT
Scenario: Rebalancer bot is started
    Given AlgoTrader is running
    And MarketService mock is running with default configuration
    And MarketService mock expects invocations on DELETE /openOrders
    When Bots page is opened
    And Bot "rebalancer" is selected
    And Rebalancer bot configuration is filled with execution period "10", quote asset "USDT", base assets ["BTC", "ETH"] with shares ["90", "10"] and confirmed
    And System runs for 5 sec
    Then MarketService method DELETE of endpoint /openOrders has been invoked with query params "{"symbol":"BTCUSDT"}"
    And MarketService method DELETE of endpoint /openOrders has been invoked with query params "{"symbol":"ETHUSDT"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"ETHUSDT", "side":"SELL", "type":"MARKET", "quoteOrderQty":"212248.000000"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"BTCUSDT", "side":"BUY", "type":"MARKET", "quoteOrderQty":"212248.000000"}"
