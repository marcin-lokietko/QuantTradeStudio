Feature: Orders page


Scenario: Active orders are presented
    Given AlgoTrader is running
    And MarketService mock is running with default configuration
    When Orders page is opened
    Then Active orders are presented
