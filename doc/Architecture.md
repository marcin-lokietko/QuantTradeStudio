# Backend components
## GuiService
Hides complexity of communicating with concrete user interfaces.

It should allow to easily change the user interface without impacting the rest of the application, so it must not contain any business logic.

It depends on:
* interfaces of: __ApiGateway__,
* components: __Http__.

## ApiGateway
Orchestrates all the main backend components of the application.

It should hide how those components interact with each other from the __GuiService__.

It depends on:
* interfaces of: __Wallet__, __MarketService__, __BotExecution__.

<!-- All the above interfaces should be defined in ApiGateway catalog, in order for the dependency inversion to be complete -->

## Wallet
Provides information about user's assets on all trading platforms.

It depends on:
* interfaces of: __MarketService__,

## MarketService
Provides information about a single trading platform. Also accepts trading requests.

It depends on:
* components: __Http__.

## BotExecution
Decides when and what market action to take - based on user's assets and market state and history.

It depends on:
* interfaces of: __MarketService__,

## Http
Provides the basic functionality of performing HTTP requests.

## Config
Reads backend json config file.

## Utils
Consists of all the minor helpers that can be used throughout the whole application. 