Feature: Misc


Scenario: Invoking /health returns OK status code
    Given MarketService mock is running with default configuration
    And Backend is available
    When Request GET /health is sent
    Then Response for GET /health was received with status code "200" and no body
