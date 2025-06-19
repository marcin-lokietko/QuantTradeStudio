message(STATUS "### Configuring unit tests...")

find_package(GTest REQUIRED CONFIG)
message(STATUS "Using compilation flags: ${COMPILE_FLAGS}")
message(STATUS "Using linking flags: ${LINKER_FLAGS}")
message(STATUS "GTest version: ${GTest_VERSION}")

# Generate common mocks
include(GenerateMock.cmake)

run_python_script(BotExecutionMock
    ${CMAKE_SOURCE_DIR}/src/BotExecution/IBotExecution.hpp
    ${CMAKE_SOURCE_DIR}/ut/GeneratedMocks/BotExecutionMock.hpp
)

run_python_script(EncryptionMock
    ${CMAKE_SOURCE_DIR}/src/MarketService/Binance/IEncryption.hpp
    ${CMAKE_SOURCE_DIR}/ut/GeneratedMocks/EncryptionMock.hpp
)

run_python_script(HttpMock
    ${CMAKE_SOURCE_DIR}/src/Http/IHttp.hpp
    ${CMAKE_SOURCE_DIR}/ut/GeneratedMocks/HttpMock.hpp
)

run_python_script(MarketServiceMock
    ${CMAKE_SOURCE_DIR}/src/MarketService/IMarketService.hpp
    ${CMAKE_SOURCE_DIR}/ut/GeneratedMocks/MarketServiceMock.hpp
)

run_python_script(TimeMock
    ${CMAKE_SOURCE_DIR}/src/Utils/Time/ITime.hpp
    ${CMAKE_SOURCE_DIR}/ut/GeneratedMocks/TimeMock.hpp
)

run_python_script(WalletMock
    ${CMAKE_SOURCE_DIR}/src/Wallet/IWallet.hpp
    ${CMAKE_SOURCE_DIR}/ut/GeneratedMocks/WalletMock.hpp
)

# Add test components
add_subdirectory(ApiGatewayUt)
add_subdirectory(BotExecutionUt)
add_subdirectory(ConfigUt)
add_subdirectory(GuiServiceUt)
add_subdirectory(MarketServiceUt)
add_subdirectory(UtilsUt)
add_subdirectory(WalletUt)

# Add main test executable
add_executable(UnitTests main.cpp)
target_compile_options(UnitTests PUBLIC ${COMPILE_FLAGS})

# Link test executable against gtest and all the test components.
# --whole-archive is used to ensure that all symbols from static libraries are included in the final binary.
# If the static libraries are not used, they might be excluded by the linker, so we need to force inclusion
# so that all tests are registered.
target_link_libraries(UnitTests PRIVATE ${LINKER_FLAGS} 
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main

    "-Wl,--whole-archive"
    ApiGatewayUt
    BotExecutionUt
    ConfigUt
    GuiServiceUt
    MarketServiceUt
    UtilsUt
    WalletUt
    "-Wl,--no-whole-archive"
)
