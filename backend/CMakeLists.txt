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

add_subdirectory(src)

#### Unit tests
if(buildUT)
    add_subdirectory(ut)
endif(buildUT)
