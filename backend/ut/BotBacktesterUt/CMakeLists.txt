add_library(BotBacktesterUt STATIC
    Evaluator/Evaluator.ut.cpp
    Simulators/MarketServiceSimulator.ut.cpp
    Simulators/SystemTimeSimulator.ut.cpp
)

#search for headers in root project directory
target_include_directories(BotBacktesterUt PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/ut
)

target_link_libraries(BotBacktesterUt
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main

    BotBacktester
)
