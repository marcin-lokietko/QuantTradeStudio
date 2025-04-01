#input parameters
option(buildType "supported types are: production, sanitizer, profiler, debug")
option(buildUt "is building unit tests")

set(buildType "" CACHE STRING "supported types are: production, sanitizer, profiler, debug")

#basics
cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
project(KanbanBoard)
set(CMAKE_CXX_COMPILER "/bin/g++")


#set compilation flags
set(EXECUTABLE_NAME "AlgoTrader")
# set(BASIC_COMPILE_FLAGS "-Wall;-Wextra;-Werror;-pthread;-lboost_system;-lrt;-lm;-ldl")
set(BASIC_COMPILE_FLAGS "-Wall;-Wextra;-Werror;-pthread;-lrt;-lm;-ldl")

set(PROD_COMPILE_FLAGS ";-O3")
set(SANITIZER_COMPILE_FLAGS ";-ggdb;-O0;-fsanitize=address,undefined")
set(PROFILER_COMPILE_FLAGS ";-O3;-pg")
set(DEBUG_COMPILE_FLAGS ";-ggdb;-O0")

if(buildType STREQUAL "production")
    message("### Running production build ###")
    set(COMPILE_FLAGS ${BASIC_COMPILE_FLAGS} ${PROD_COMPILE_FLAGS})
elseif(buildType STREQUAL "sanitizer")
    message("### Running sanitizer build ###")
    set(COMPILE_FLAGS ${BASIC_COMPILE_FLAGS} ${SANITIZER_COMPILE_FLAGS})
    set(LINKER_FLAGS "-fsanitize=address,undefined")
elseif(buildType STREQUAL "profiler")
    message("### Running profiler build ###")
    set(COMPILE_FLAGS ${BASIC_COMPILE_FLAGS} ${PROFILER_COMPILE_FLAGS})
    set(LINKER_FLAGS "-pg")
elseif(buildType STREQUAL "debug")
    message("### Running debug build ###")
    set(COMPILE_FLAGS ${BASIC_COMPILE_FLAGS} ${DEBUG_COMPILE_FLAGS})
else()
    message( FATAL_ERROR "Invalid buildType configured, CMake will exit." )
endif()


message(STATUS "Configured sources directory: ${PROJECT_SOURCE_DIR}")
message(STATUS "Configured binary output directory: ${CMAKE_CURRENT_BINARY_DIR}")


# #Boost libraries
# set(Boost_USE_STATIC_LIBS OFF)
# set(Boost_USE_MULTITHREADED ON)
# set(Boost_USE_STATIC_RUNTIME OFF)
# find_package(Boost 1.79 REQUIRED COMPONENTS thread)
# message(STATUS "Boost version: ${Boost_VERSION}")

#Curl library
find_package(CURL REQUIRED)

#Openssl library
find_package(OpenSSL REQUIRED)

#Add components
add_subdirectory(Wallet)
add_subdirectory(GuiService)
add_subdirectory(Http)
add_subdirectory(MarketService)
add_subdirectory(ApiGateway)

#declare the target
add_executable(${EXECUTABLE_NAME}
    ${PROJECT_SOURCE_DIR}/main.cpp
)

#include directories for headers
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

#compile and link
message(STATUS "Using compilation flags: ${COMPILE_FLAGS}")
message(STATUS "Using linking flags: ${LINKER_FLAGS}")

target_compile_options(${EXECUTABLE_NAME} PUBLIC ${COMPILE_FLAGS})
target_link_libraries(${EXECUTABLE_NAME} ${LINKER_FLAGS} 
    glog
    CURL::libcurl
    OpenSSL::SSL
    OpenSSL::Crypto
    Wallet
    GuiService
    Http
    MarketService
    ApiGateway
)

#### Unit tests
if(buildUT)
    find_package(GTest REQUIRED CONFIG)
    message(STATUS "GTest version: ${GTest_VERSION}")

    enable_testing()

    # Add test cpp files
    add_executable(unitTests utMain.cpp ${PROJECT_SOURCE_DIR}/TestDummy.cpp)
    target_compile_options(unitTests PUBLIC ${COMPILE_FLAGS})
    # Link test executable against gmock and gtest
    target_link_libraries(unitTests PRIVATE ${LINKER_FLAGS} GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
    add_test(runUnitTests runUnitTests)
endif(buildUT)
