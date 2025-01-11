add_library(StockMarketService STATIC
    Binance/BinanceService.cpp
    Binance/Encryption.cpp
)

#Openssl library
find_package(OpenSSL REQUIRED)

# include directories
target_include_directories(StockMarketService PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(StockMarketService
    glog
    CURL::libcurl
    OpenSSL::SSL
    OpenSSL::Crypto
    Http
)
