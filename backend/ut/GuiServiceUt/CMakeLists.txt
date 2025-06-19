add_library(GuiServiceUt STATIC
    HttpGuiService/Conversion/Assets.ut.cpp
    HttpGuiService/Conversion/AssetSymbols.ut.cpp
    HttpGuiService/Conversion/AvailableBaseAssetsRequest.ut.cpp
    HttpGuiService/Conversion/AvailableQuoteAssets.ut.cpp
    HttpGuiService/Conversion/AvailableQuoteAssetsRequest.ut.cpp
    HttpGuiService/Conversion/BotConfig.ut.cpp
    HttpGuiService/Conversion/OrderRequest.ut.cpp
    HttpGuiService/Conversion/Orders.ut.cpp
)

#search for headers in root project directory
target_include_directories(GuiServiceUt PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/ut
)

target_link_libraries(GuiServiceUt
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    GuiService
)
