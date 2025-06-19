add_library(UtilsUt STATIC
    StrongType.ut.cpp
    Time.ut.cpp
)

#search for headers in root project directory
target_include_directories(UtilsUt PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/ut
)

target_link_libraries(UtilsUt
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    Utils
)
