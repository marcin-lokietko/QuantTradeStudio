Feature: Rebalancer Bot


# With the default configuration of MarketService, the user owns
#  1. 123.4 ETH, which is worth 123.4*2000=246 800 USDT
#  2. 1.234 BTC, which is worth 1.234*80000=98 720 USDT
# So the total worth of the owned ETH and BTC is 345 520 USDT
# With the expected value proportion of BTC/ETH= 9/1, the expected wallet is:
#  1. 34 552 USDT in ETH => need to sell ETH worth 246 800 - 34 552 = 212248 USDT
#  2. need to buy BTC worth 212248 USDT
Scenario: Single rebalancing of 2 assets
    Given MarketService mock is running with default configuration
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    And Backend is available
    When Request POST /startBot is sent with body "{"botName":"Rebalancer","executionPeriod":10,"isExecutedImmediately":true,"quoteAsset":"USDT","baseAssetShares":[{"assetSymbol":"ETH","expectedShare":10},{"assetSymbol":"BTC","expectedShare":90}]}"
    Then Response for POST /startBot was received with status code "200" and no body
    And System runs for 5 sec
    Then MarketService method DELETE of endpoint /openOrders has been invoked with query params "{"symbol":"BTCUSDT"}"
    And MarketService method DELETE of endpoint /openOrders has been invoked with query params "{"symbol":"ETHUSDT"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"ETHUSDT", "side":"SELL", "type":"MARKET", "quoteOrderQty":"212248.000000"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"BTCUSDT", "side":"BUY", "type":"MARKET", "quoteOrderQty":"212248.000000"}"


# The user owns
#  1. 50 ETH,   worth 100 000 USDT
#  2. 1000 SOL, worth 100 000 USDT
#  3. 1 BTC,    worth 80 000 USDT
# So the total worth is 280 000 USDT
# With the expected value proportion of ETH/SOL/BTC= 2/3/5, the expected wallet is:
#  1. 56 000 USDT in ETH   => need to sell 44 000 USDT
#  2. 84 000 USDT in SOL   => need to sell 16 000 USDT
#  3. 140 000 USDT in BTC  => need to buy 60 000 USDT
Scenario: Single rebalancing of 3 assets
    Given MarketService mock is running
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    And MarketService mock expects invocations on POST /order and will return empty response
    And MarketService mock expects invocations on GET /ticker/price and will return "[{"price":"80000","symbol":"BTCUSDT"},{"price":"100","symbol":"SOLUSDT"},{"price":"2000","symbol":"ETHUSDT"},{"price":"70000","symbol":"BTCEUR"},{"price":"1500","symbol":"ETHEUR"}]"
    And MarketService mock expects invocations on GET /exchangeInfo and will return "{"symbols":[{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"},{"symbol":"SOLUSDT","baseAsset":"SOL","quoteAsset":"USDT"},{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"},{"symbol":"BTCEUR","baseAsset":"BTC","quoteAsset":"EUR"},{"symbol":"ETHEUR","baseAsset":"ETH","quoteAsset":"EUR"}]}"
    And MarketService mock expects invocations on GET /openOrders and will return "[{"symbol":"ETHUSDT","orderId":2,"price":"2000","origQty":"2","executedQty":"1","side":"SELL"}]"
    And MarketService mock expects invocations on GET /account and will return "{"balances":[{"free":"1","asset":"BTC"},{"free":"1000","asset":"SOL"},{"free":"50","asset":"ETH"}]}"
    And Backend is available
    When Request POST /startBot is sent with body "{"botName":"Rebalancer","executionPeriod":10,"isExecutedImmediately":true,"quoteAsset":"USDT","baseAssetShares":[{"assetSymbol":"ETH","expectedShare":20},{"assetSymbol":"SOL","expectedShare":30},{"assetSymbol":"BTC","expectedShare":50}]}"
    Then Response for POST /startBot was received with status code "200" and no body
    And System runs for 5 sec
    And MarketService method DELETE of endpoint /openOrders has been invoked with query params "{"symbol":"ETHUSDT"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"ETHUSDT","side":"SELL","type":"MARKET","quoteOrderQty":"44000.000000"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"SOLUSDT","side":"SELL","type":"MARKET","quoteOrderQty":"16000.000000"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"BTCUSDT","side":"BUY","type":"MARKET","quoteOrderQty":"60000.000000"}"


# 1st rebalance:
# User owns:
#  1. 50 ETH,   worth 100 000 USDT
#  2. 1 BTC,    worth 80 000 USDT
#  Total: 180 000 USDT
# With the expected value proportion of ETH/BTC= 6/4, the expected wallet is:
#  1. 108 000 USDT in ETH   => need to buy 8 000 USDT
#  2. 72 000 USDT in BTC    => need to sell 8 000 USDT
Scenario: Two rebalances of 2 assets
    Given MarketService mock is running
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    And MarketService mock expects invocations on POST /order and will return empty response
    And MarketService mock expects invocations on GET /ticker/price and will return "[{"price":"80000","symbol":"BTCUSDT"},{"price":"2000","symbol":"ETHUSDT"}]"
    And MarketService mock expects invocations on GET /exchangeInfo and will return "{"symbols":[{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"},{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"}]}"
    And MarketService mock expects invocations on GET /openOrders and will return "[{"symbol":"ETHUSDT","orderId":2,"price":"2000","origQty":"2","executedQty":"1","side":"SELL"}]"
    And MarketService mock expects invocations on GET /account and will return "{"balances":[{"free":"1","asset":"BTC"},{"free":"50","asset":"ETH"}]}"
    And Backend is available
    When Request POST /startBot is sent with body "{"botName":"Rebalancer","executionPeriod":10,"isExecutedImmediately":true,"quoteAsset":"USDT","baseAssetShares":[{"assetSymbol":"ETH","expectedShare":60},{"assetSymbol":"BTC","expectedShare":40}]}"
    Then Response for POST /startBot was received with status code "200" and no body
    And System runs for 5 sec
    And MarketService method DELETE of endpoint /openOrders has been invoked with query params "{"symbol":"ETHUSDT"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"ETHUSDT","side":"BUY","type":"MARKET","quoteOrderQty":"8000.000000"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"BTCUSDT","side":"SELL","type":"MARKET","quoteOrderQty":"8000.000000"}"

# 2nd rebalance:
# User owns:
#  1. 54 ETH,     worth 54 000 USDT
#  2. 1.1 BTC,    worth 110 000 USDT
#  Total: 164 000 USDT
# With the expected value proportion of ETH/BTC= 6/4, the expected wallet is:
#  1. 98 400 USDT in ETH   => need to buy 44 400 USDT
#  2. 65 600 USDT in BTC   => need to sell 44 400 USDT
    And MarketService mock is reset
    And MarketService mock expects invocations on DELETE /openOrders and will return empty response
    And MarketService mock expects invocations on POST /order and will return empty response
    And MarketService mock expects invocations on GET /ticker/price and will return "[{"price":"100000","symbol":"BTCUSDT"},{"price":"1000","symbol":"ETHUSDT"}]"
    And MarketService mock expects invocations on GET /exchangeInfo and will return "{"symbols":[{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"},{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"}]}"
    And MarketService mock expects invocations on GET /openOrders and will return "[{"symbol":"ETHUSDT","orderId":2,"price":"2000","origQty":"2","executedQty":"1","side":"SELL"}]"
    And MarketService mock expects invocations on GET /account and will return "{"balances":[{"free":"1.1","asset":"BTC"},{"free":"54","asset":"ETH"}]}"
    And System runs for 10 sec
    And MarketService method DELETE of endpoint /openOrders has been invoked with query params "{"symbol":"ETHUSDT"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"ETHUSDT","side":"BUY","type":"MARKET","quoteOrderQty":"44400.000000"}"
    And MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"BTCUSDT","side":"SELL","type":"MARKET","quoteOrderQty":"44400.000000"}"
