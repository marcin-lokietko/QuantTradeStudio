add_library(MarketServiceUt STATIC
    Binance/BinanceService.ut.cpp
)

#search for headers in root project directory
target_include_directories(MarketServiceUt PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/ut
)

target_link_libraries(MarketServiceUt
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    MarketService
)
