Feature: Trading

Scenario: Making an order
    Given Backend is available
    And MarketService mock is running
    When GET /assets request is sent
    Then GET /assets response is valid
