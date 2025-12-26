# Backend components

## GuiService

Hides complexity of communicating with concrete user interfaces.

It should allow to easily change the user interface without impacting the rest of the application, so it must not contain any business logic.

Depends on:

- interfaces of: **ApiGateway**

## ApiGateway

Orchestrates all the main backend components of the application.

It should hide how those components interact with each other from the **GuiService**.

Depends on:

- interfaces of: **MarketService**, **BotBacktester**, **BotExecution**

## MarketService

Provides information about a trading platform and handles trading requests.

Currently implements Binance integration.

Depends on:

- components: **Http**

## BotAlgorithms

Defines trading bots which are used in BotBacktester and BotExecution components. Each trading bot decides when and what market action to take - based on user's assets and market state and history.

Depends on:

- interfaces of: **MarketService**

## BotExecution

Initiates and ends bot execution.

Depends on:

- interfaces of: **MarketService**
- components: **BotAlgorithms**

## BotBacktester

Initiates the execution of the bot on historical market data and evaluates the results.

Depends on:

- interfaces of: **MarketService**
- components: **BotAlgorithms**

## Http

Provides the basic functionality of performing HTTP requests.

Depends on:

- (none)

## Config

Reads the backend JSON config file. This centralizes configuration and avoids reliance on command line arguments.

Depends on:

- (none)

## Utils

Consists of all the minor helpers that can be used throughout the whole application. Those functions/classes must not depend on any other component/interface.

Depends on:

- (none)

# General notes

- A namespace should correspond to the path to the file. The exception is the Utils component - using a namespace there would clutter the code.
- The strong types/boxed types are used for most of the public methods' parameters. The main goals are:
  - avoiding catastrophic bugs, caused by confusing things like: Price vs Amount,
  - When code is reviewed, strong types encode intent.
- For more clarity, commit messages should start with one of: [BE], [FE], [TEST], [CI], [DOC].
- 2025-06-21 Migrated from glog to spdlog. The advantages of spdlog over glog are:
  - More modern interface: config is not done via global variables; does not use operator<<
  - Does not create a file per log level like glog does. This was misleading and annoying quirk of glog
- The following files must remain thin because they're coupled to external libraries and hard to unit test:
  - HttpGuiService.cpp (Crow)
  - Http.cpp (Curl)
  - Encryption.cpp (OpenSSL)
