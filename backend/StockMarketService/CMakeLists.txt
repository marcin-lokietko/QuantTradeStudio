add_library(StockMarketService STATIC
    Binance/BinanceService.cpp
    Binance/Encryption.cpp
)

#Openssl library
find_package(OpenSSL REQUIRED)

#search for headers in root project directory
target_include_directories(StockMarketService PRIVATE ${PROJECT_SOURCE_DIR})

target_compile_options(StockMarketService PUBLIC ${COMPILE_FLAGS})

target_link_libraries(StockMarketService ${LINKER_FLAGS}
    glog
    CURL::libcurl
    OpenSSL::SSL
    OpenSSL::Crypto
    Http
)
