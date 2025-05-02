Feature: Trading

Scenario: Making an order
    Given Backend is available
    When Order request is sent
    Then Order response is received