add_library(StockMarketService STATIC
    BinanceService.cpp
)

#Curl library
find_package(CURL REQUIRED)

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
