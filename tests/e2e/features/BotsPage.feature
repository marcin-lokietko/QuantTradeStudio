Feature: Bots page


# See tests/backend/features/RebalancerBot.feature, scenario "Single rebalancing of 2 assets" for explanation of the expected requests to MarketService
Scenario: Rebalancer bot is started
    Given QuantTradeStudio is running
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
    Given QuantTradeStudio is running
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


# See tests/backend/features/MovingAverageCrossoverBot.feature, scenario outline "Ordering a trade when short and long averages cross over" - example 1
Scenario: Moving Average Crossover bot is started
    Given QuantTradeStudio is running
    And MarketService mock is running
    And MarketService mock expects invocations on GET /openOrders and will return "[{"symbol":"BTCUSDT","orderId":2,"price":"80000","origQty":"0.01","executedQty":"0","side":"SELL"}]"
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    And MarketService mock expects invocations on POST /order and will return empty response
    And MarketService mock expects invocations on GET /ticker/price and will return responses as defined in file "price/3DayAverageRisesAbove6DayAverage.json"
    And MarketService mock expects invocations on GET /exchangeInfo and will return "{"symbols":[{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"},{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"}]}"
    And MarketService mock expects invocations on GET /account and will return "{"balances":[{"free":"1","asset":"BTC"},{"free":"50","asset":"ETH"},{"free":"10000","asset":"USDT"}]}"
    When Bots page is opened
    And Bot "MovingAverageCrossover" is selected
    And Moving Average Crossover bot configuration is filled with execution period "1", short average length "3", long average length "6", quote asset "USDT", base assets ["BTC", "ETH"]
    And Bot launch button is clicked
    And System runs for 7.5 sec
    Then MarketService method DELETE of endpoint /openOrders has been invoked with query params "{"symbol":"BTCUSDT"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"BTCUSDT", "side":"BUY", "type":"MARKET", "quoteOrderQty":"10000"}"


# See tests/backend/features/MovingAverageCrossoverBotbacktesting.feature, scenario "Backtesting of ordering a trade when short and long averages cross over"
Scenario: Backtest of Moving Average Crossover bot is started
    Given QuantTradeStudio is running
    Given MarketService mock is running
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    And MarketService mock expects invocations on GET /exchangeInfo and will return "{"symbols":[{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"},{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"}]}"
    And MarketService mock expects invocations on GET /klines and will return responses as defined in file "klines/3DayAverageFallsBelow6DayAverage.json"
    When Bots page is opened
    And Bot "MovingAverageCrossover" is selected
    And Moving Average Crossover bot configuration is filled with execution period "61", short average length "3", long average length "6", quote asset "USDT", base assets ["BTC", "ETH"]
    And Open bot backtest dialog button is clicked
    And Backtest configuration is filled with transaction fee percent "1", owned assets ["BTC", "ETH", "USDT"] with amounts ["1", "10", "10000"] respectively, simulation start date "10/17/2025", start time "17:59", simulation end date "10/17/2025", end time "18:08"
    And Launch backtest button is clicked
    Then Total profit if held "-1,100.00" is shown
    And Total profit "-795.00" is shown


# See tests/backend/features/DonchianChannelBreakoutStrategy.feature, scenario outline "Ordering a trade when current close price is more extreme than exit/entry channel extremum" - example 1
Scenario: Donchian Channel Breakout Strategy bot is started
    Given QuantTradeStudio is running
    And MarketService mock is running
    And MarketService mock expects invocations on GET /openOrders and will return "[{"symbol":"BTCUSDT","orderId":2,"price":"80000","origQty":"0.01","executedQty":"0","side":"SELL"}]"
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    And MarketService mock expects invocations on POST /order and will return empty response
    And MarketService mock expects invocations on GET /klines and will return responses as defined in file "klines/currentClosePriceIsHigherThanEntryChannelMaximum.json"
    And MarketService mock expects invocations on GET /exchangeInfo and will return "{"symbols":[{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"},{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"}]}"
    And MarketService mock expects invocations on GET /account and will return "{"balances":[{"free":"1","asset":"BTC"},{"free":"50","asset":"ETH"},{"free":"10000","asset":"USDT"}]}"
    When Bots page is opened
    And Bot "DonchianChannelBreakoutStrategy" is selected
    And Moving Donchian Channel Breakout Strategy bot configuration is filled with execution interval "OneHour", exit channel length "3", entry channel length "6", quote asset "USDT", base assets ["BTC", "ETH"]
    And Bot launch button is clicked
    And System runs for 1 sec
    And MarketService method DELETE of endpoint /openOrders has been invoked with query params "{"symbol":"BTCUSDT"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"BTCUSDT", "side":"BUY", "type":"MARKET", "quoteOrderQty":"10000"}"


# See tests/backend/features/DonchianChannelBreakoutStrategyBacktesting.feature
Scenario: Backtest of Donchian Channel Breakout Strategy bot is started
    Given QuantTradeStudio is running
    Given MarketService mock is running
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    And MarketService mock expects invocations on GET /exchangeInfo and will return "{"symbols":[{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"},{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"}]}"
    And MarketService mock expects invocations on GET /klines and will return responses as defined in file "klines/currentClosePriceIsHigherThanEntryChannelMaximum2.json"
    When Bots page is opened
    And Bot "DonchianChannelBreakoutStrategy" is selected
    And Moving Donchian Channel Breakout Strategy bot configuration is filled with execution interval "OneMinute", exit channel length "3", entry channel length "6", quote asset "USDT", base assets ["BTC", "ETH"]
    And Open bot backtest dialog button is clicked
    And Backtest configuration is filled with transaction fee percent "1", owned assets ["BTC", "ETH", "USDT"] with amounts ["1", "10", "10000"] respectively, simulation start date "10/17/2025", start time "18:07", simulation end date "10/17/2025", end time "18:08"
    And Launch backtest button is clicked
    Then Total profit if held "2,000.00" is shown
    And Total profit "2,138.55" is shown
