message(STATUS "### Configuring unit tests...")

find_package(GTest REQUIRED CONFIG)
message(STATUS "Using compilation flags: ${COMPILE_FLAGS}")
message(STATUS "Using linking flags: ${LINKER_FLAGS}")
message(STATUS "GTest version: ${GTest_VERSION}")

# Generate common mocks
include(GenerateMock.cmake)

run_python_script(WalletMock
    ${CMAKE_SOURCE_DIR}/src/Wallet/IWallet.hpp
    ${CMAKE_SOURCE_DIR}/ut/CommonMocks/WalletMock.hpp
)

run_python_script(TimeMock
    ${CMAKE_SOURCE_DIR}/src/Utils/Time/ITime.hpp
    ${CMAKE_SOURCE_DIR}/ut/CommonMocks/TimeMock.hpp
)

run_python_script(MarketServiceMock
    ${CMAKE_SOURCE_DIR}/src/MarketService/IMarketService.hpp
    ${CMAKE_SOURCE_DIR}/ut/CommonMocks/MarketServiceMock.hpp
)

# Add test components
add_subdirectory(BotExecutionUt)

# Add main test executable
add_executable(UnitTests main.cpp)
target_compile_options(UnitTests PUBLIC ${COMPILE_FLAGS})

# Link test executable against gtest and all the test components
target_link_libraries(UnitTests PRIVATE ${LINKER_FLAGS} 
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    BotExecutionUt
)
