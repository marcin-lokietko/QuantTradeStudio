add_library(BotAlgorithmsUt STATIC
    ConfigExtractor.ut.cpp
    Rebalancer.ut.cpp
)

#search for headers in root project directory
target_include_directories(BotAlgorithmsUt PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/ut
)

target_link_libraries(BotAlgorithmsUt
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main

    BotAlgorithms
)
