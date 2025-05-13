Feature: Assets page

Scenario: Presenting assets
    Given Backend is available
    And Frontend is available
    And MarketService mock is running
    When Assets page is opened
    Then Assets are presented
