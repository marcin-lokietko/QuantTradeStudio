add_library(Http STATIC
    Http.cpp
)

#Curl library
find_package(CURL REQUIRED)

#Openssl library
find_package(OpenSSL REQUIRED)

#search for headers in root project directory
target_include_directories(Http PRIVATE ${PROJECT_SOURCE_DIR})

target_compile_options(Http PUBLIC ${COMPILE_FLAGS})

target_link_libraries(Http ${LINKER_FLAGS}
    glog
    CURL::libcurl
    OpenSSL::SSL
    OpenSSL::Crypto
)
