Feature: Assets page


Scenario: Owned assets are presented
    Given AlgoTrader is running
    And MarketService mock is running with default configuration
    When Assets page is opened
    Then Assets are presented


Scenario Outline: Asset order is placed
    Given AlgoTrader is running
    And MarketService mock is running with default configuration
    When Assets page is opened
    And Assets are presented
    And Make Order dialog is opened for "<base_asset>"
    And Make Order dialog is filled with quote asset "<quote_asset>", order side "<side>", amount "<amount>" and confirmed
    And System runs for 1 sec
    Then MarketService method POST of endpoint /order has been invoked with query params "{"symbol":"<trading_pair>","side":"<side>","type":"LIMIT","timeInForce":"GTC","quantity":"<amount>","price":"","recvWindow":"5000"}"
    Examples:
      | base_asset | quote_asset | side | amount | trading_pair |
      | ETH        | BTC         | BUY  | 0.1    | ETHBTC       |
      | ETH        | USDT        | BUY  | 0.2    | ETHUSDT      |
      | ETH        | BTC         | SELL | 0.3    | ETHBTC       |
