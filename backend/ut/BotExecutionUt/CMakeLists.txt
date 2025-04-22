add_library(BotExecutionUt STATIC
    Rebalancer.ut.cpp
)

#search for headers in root project directory
target_include_directories(BotExecutionUt PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(BotExecutionUt
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    BotExecution
)

# GoogleTest setup
