add_library(Http STATIC
    Http.cpp
)

#Curl library
find_package(CURL REQUIRED)

#Openssl library
find_package(OpenSSL REQUIRED)

# include directories
target_include_directories(Http PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(Http
    glog
    CURL::libcurl
    OpenSSL::SSL
    OpenSSL::Crypto
)
