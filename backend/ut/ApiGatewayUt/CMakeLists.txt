add_library(ApiGatewayUt STATIC
    ApiGateway.ut.cpp
)

#search for headers in root project directory
target_include_directories(ApiGatewayUt PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/ut
)

target_link_libraries(ApiGatewayUt
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    ApiGateway
)
