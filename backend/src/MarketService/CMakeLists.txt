add_library(MarketService STATIC
    Binance/BinanceService.cpp
    Binance/Conversion/AssetPrices.cpp
    Binance/Conversion/Assets.cpp
    Binance/Conversion/Orders.cpp
    Binance/Conversion/TradingPairs.cpp
    Binance/Encryption.cpp
)

#Openssl library
find_package(OpenSSL REQUIRED)

#search for headers in root project directory
target_include_directories(MarketService PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_compile_options(MarketService PUBLIC ${COMPILE_FLAGS})

target_link_libraries(MarketService ${LINKER_FLAGS}
    CURL::libcurl
    OpenSSL::Crypto
    OpenSSL::SSL
    spdlog

    Http
)
