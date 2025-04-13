add_library(GuiService STATIC
  HttpGuiService/Conversion/Assets.cpp
  HttpGuiService/Conversion/AssetSymbols.cpp
  HttpGuiService/Conversion/AvailableBaseAssetsRequest.cpp
  HttpGuiService/Conversion/AvailableQuoteAssets.cpp
  HttpGuiService/Conversion/AvailableQuoteAssetsRequest.cpp
  HttpGuiService/Conversion/BotConfig.cpp
  HttpGuiService/Conversion/OrderRequest.cpp
  HttpGuiService/Conversion/Orders.cpp
  HttpGuiService/HttpGuiService.cpp
)

#search for headers in root project directory
target_include_directories(GuiService PRIVATE ${PROJECT_SOURCE_DIR})

target_compile_options(GuiService PUBLIC ${COMPILE_FLAGS})

#Crow library
find_package( Crow REQUIRED)
set_target_properties(Crow::Crow PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "" #workaround for "ISO C++11 requires whitespace after the macro name" warning when linking Crow
)

target_link_libraries(GuiService ${LINKER_FLAGS}
  Crow::Crow
  glog
)
