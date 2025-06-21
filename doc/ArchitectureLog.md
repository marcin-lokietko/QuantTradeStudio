This file is supposed to present the rationale behind the significant design changes done during the development, as the architectural vision emerges.

[30.03.2025/Marcin Lokietko]\
Added Architecture.md with initial sketch of the backend architecture. It mostly documents current structure of the project, but also introduces components to be implemented in the future:

- ApiGateway - needed not to pollute GuiService with domain logic
- BotExecution - component for deciding which assets to buy and when

[3.05.2025/Marcin Lokietko]\
Added Config component that reads json configuration. The goal was to avoid adding more command line arguments to the backend application. Also added Binance URL config parameter, which will later allow to mock the market service in backend component tests.

[21.06.2025/Marcin Lokietko]\
Migrated from glog to spdlog. The advantages of spdlog over glog are:

- More modern interface: config is not done via global variables; does not use operator<<
- Has more log-levels
- Does not create a file per log level like glog does. This was misleading and annoying quirk of glog
