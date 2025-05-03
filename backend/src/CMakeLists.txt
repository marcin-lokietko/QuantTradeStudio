message(STATUS "### Configuring implementation...")
message(STATUS "Configured sources directory: ${PROJECT_SOURCE_DIR}")
message(STATUS "Configured binary output directory: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "Using compilation flags: ${COMPILE_FLAGS}")
message(STATUS "Using linking flags: ${LINKER_FLAGS}")

#Curl library
find_package(CURL REQUIRED)

#Openssl library
find_package(OpenSSL REQUIRED)

#Add components
add_subdirectory(ApiGateway)
add_subdirectory(BotExecution)
add_subdirectory(Config)
add_subdirectory(GuiService)
add_subdirectory(Http)
add_subdirectory(MarketService)
add_subdirectory(Utils)
add_subdirectory(Wallet)

#declare the target
add_executable(${EXECUTABLE_NAME}
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

#include directories for headers
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)

#compile and link static libraries to the executable
target_compile_options(${EXECUTABLE_NAME} PUBLIC ${COMPILE_FLAGS})
target_link_libraries(${EXECUTABLE_NAME} ${LINKER_FLAGS} 
    CURL::libcurl
    glog
    OpenSSL::Crypto
    OpenSSL::SSL

    ApiGateway
    BotExecution
    Config
    GuiService
    Http
    MarketService
    Wallet
)
