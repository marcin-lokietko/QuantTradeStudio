add_library(ConfigUt STATIC
    Conversion/ConfigParams.ut.cpp
)

#search for headers in root project directory
target_include_directories(ConfigUt PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/ut
)

target_link_libraries(ConfigUt
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    Config
)
