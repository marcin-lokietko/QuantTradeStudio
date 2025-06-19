add_library(WalletUt STATIC
    Wallet.ut.cpp
)

#search for headers in root project directory
target_include_directories(WalletUt PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/ut
)

target_link_libraries(WalletUt
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    Wallet
)
