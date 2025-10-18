Feature: MarketData


Scenario Outline: Invoking /availableQuoteAssets returns assets
    Given MarketService mock is running
    And MarketService mock expects invocations on GET /exchangeInfo and will return "<mocked_exchange_info>"
    And MarketService mock expects invocations on GET /ticker/price and will return "<mocked_price>"
    And Backend is available
    When Request GET /availableQuoteAssets is sent with query "<sent_query>"
    Then Response for GET /availableQuoteAssets was received with status code "200" and body "<received_quote_assets>"
    Examples:
      | mocked_exchange_info                                                                                                                                                                                                                                  | mocked_price                                                                                                                                      | sent_query      | received_quote_assets                                                                                            |
      | {"symbols":[{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"},{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"}]}                                                                                                                   | [{"price":"80000","symbol":"BTCUSDT"},{"price":"2000","symbol":"ETHUSDT"}]                                                                        | baseAsset=      | []                                                                                                   |
      | {"symbols":[{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"},{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"}]}                                                                                                                   | [{"price":"80000","symbol":"BTCUSDT"},{"price":"2000","symbol":"ETHUSDT"}]                                                                        | baseAsset=DUMMY | []                                                                                                   |
      | {"symbols":[{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"},{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"}]}                                                                                                                   | [{"price":"80000","symbol":"BTCUSDT"},{"price":"2000","symbol":"ETHUSDT"}]                                                                        | baseAsset=BTC   | [{"baseAssetUnitPrice":"80000","quoteAsset":"USDT"}]                                                 |
      | {"symbols":[{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"},{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"}]}                                                                                                                   | [{"price":"80000","symbol":"BTCUSDT"},{"price":"2000","symbol":"ETHUSDT"}]                                                                        | baseAsset=ETH   | [{"baseAssetUnitPrice":"2000","quoteAsset":"USDT"}]                                                  |
      | {"symbols":[{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"}]}                                                                                                                                                                              | [{"price":"80000","symbol":"BTCUSDT"}]                                                                                                            | baseAsset=ETH   | []                                                                                                   |
      | {"symbols":[{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"}]}                                                                                                                                                                              | [{"price":"2000","symbol":"ETHUSDT"}]                                                                                                             | baseAsset=ETH   | [{"baseAssetUnitPrice":"2000","quoteAsset":"USDT"}]                                                  |
      | {"symbols":[]}                                                                                                                                                                                                                                        | []                                                                                                                                                | baseAsset=ETH   | []                                                                                                   |
      | {"symbols":[{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"},{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"},{"symbol":"BTCEUR","baseAsset":"BTC","quoteAsset":"EUR"},{"symbol":"ETHEUR","baseAsset":"ETH","quoteAsset":"EUR"}]} | [{"price":"80000","symbol":"BTCUSDT"},{"price":"2000","symbol":"ETHUSDT"},{"price":"70000","symbol":"BTCEUR"},{"price":"1500","symbol":"ETHEUR"}] | baseAsset=ETH   | [{"baseAssetUnitPrice":"2000","quoteAsset":"USDT"},{"baseAssetUnitPrice":"1500","quoteAsset":"EUR"}] |


Scenario Outline: Invoking /quoteAssetsSuitableForRebalancing returns assets
    Given MarketService mock is running
    And MarketService mock expects invocations on GET /exchangeInfo and will return "<mocked_exchange_info>"
    And Backend is available
    When Request GET /quoteAssetsSuitableForRebalancing is sent with query "<sent_query>"
    Then Response for GET /quoteAssetsSuitableForRebalancing was received with status code "200" and body "<received_quote_assets>"
    Examples:
      | mocked_exchange_info                                                                                                                                                                         | received_quote_assets    |
      | {"symbols":[{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"},{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"}]}                                                          | [{"assetSymbol":"USDT"}] |
      | {"symbols":[{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"},{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"},{"symbol":"BTCEUR","baseAsset":"BTC","quoteAsset":"EUR"}]} | [{"assetSymbol":"USDT"}] |
      | {"symbols":[{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"}]}                                                                                                                     | []                       |
      | {"symbols":[]}                                                                                                                                                                               | []                       |


Scenario Outline: Invoking /availableBaseAssets returns assets
    Given MarketService mock is running
    And MarketService mock expects invocations on GET /exchangeInfo and will return "<mocked_exchange_info>"
    And Backend is available
    When Request GET /availableBaseAssets is sent with query "<sent_query>"
    Then Response for GET /availableBaseAssets was received with status code "200" and body "<received_base_assets>"
    Examples:
      | mocked_exchange_info                                                                                                                                                                         | sent_query            | received_base_assets                          |
      | {"symbols":[{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"},{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"}]}                                                          | noQuoteAssetProvided= | [{"assetSymbol":"BTC"},{"assetSymbol":"ETH"}] |
      | {"symbols":[{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"},{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"}]}                                                          | quoteAsset=DUMMY      | []                                            |
      | {"symbols":[{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"},{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"}]}                                                          | quoteAsset=USDT       | [{"assetSymbol":"BTC"},{"assetSymbol":"ETH"}] |
      | {"symbols":[{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"},{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"},{"symbol":"BTCEUR","baseAsset":"BTC","quoteAsset":"EUR"}]} | quoteAsset=USDT       | [{"assetSymbol":"BTC"},{"assetSymbol":"ETH"}] |
      | {"symbols":[{"symbol":"BTCUSDT","baseAsset":"BTC","quoteAsset":"USDT"},{"symbol":"ETHUSDT","baseAsset":"ETH","quoteAsset":"USDT"},{"symbol":"BTCEUR","baseAsset":"BTC","quoteAsset":"EUR"}]} | quoteAsset=EUR        | [{"assetSymbol":"BTC"}]                       |
