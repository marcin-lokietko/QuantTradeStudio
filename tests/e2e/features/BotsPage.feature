Feature: Bots page


# See tests/backend/features/RebalancerBot.feature, scenario "Single rebalancing of 2 assets" for explanation of the expected requests to MarketService
Scenario: Rebalancer bot is started
    Given AlgoTrader is running
    And MarketService mock is running with default configuration
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    When Bots page is opened
    And Bot "rebalancer" is selected
    And Rebalancer bot configuration is filled with execution period "10", quote asset "USDT", base assets ["BTC", "ETH"] with shares ["90", "10"]
    And Bot launch button is clicked
    And System runs for 5 sec
    Then MarketService method DELETE of endpoint /openOrders has been invoked with query params "{"symbol":"BTCUSDT"}"
    And MarketService method DELETE of endpoint /openOrders has been invoked with query params "{"symbol":"ETHUSDT"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"ETHUSDT", "side":"SELL", "type":"MARKET", "quoteOrderQty":"212248.000000"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"BTCUSDT", "side":"BUY", "type":"MARKET", "quoteOrderQty":"212248.000000"}"


# See tests/backend/features/RebalancerBotBacktesting.feature, scenario "Backtesting single rebalancing of 2 assets"
Scenario: Backtest of Rebalancer bot is started
    Given AlgoTrader is running
    And MarketService mock is running with default configuration
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    And MarketService mock expects invocations on GET /klines and will return responses as defined in file "klines/1.json"
    When Bots page is opened
    And Bot "rebalancer" is selected
    And Rebalancer bot configuration is filled with execution period "60", quote asset "USDT", base assets ["BTC", "ETH"] with shares ["90", "10"]
    And Open bot backtest dialog button is clicked
    And Backtest configuration is filled with transaction fee percent "1", owned assets ["BTC", "ETH", "USDT"] with amounts ["1.234", "123.4", "10000"] respectively, simulation start date "10/17/2025", start time "18:00", simulation end date "10/17/2025", end time "18:02"
    And Launch backtest button is clicked
    Then Total profit if held "0.00" is shown
    And Total profit "-4,244.96" is shown
