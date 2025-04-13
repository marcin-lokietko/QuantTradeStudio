This file is supposed to present the rationale behind the significant design changes done during the development, as the architectural vision emerges.

[30.03.2025/Marcin Lokietko]\
Added Architecture.md with initial sketch of the backend architecture. It mostly documents current structure of the project, but also introduces components to be implemented in the future:
* ApiGateway - needed not to pollute GuiService with domain logic
* BotExecution - component for deciding which assets to buy and when