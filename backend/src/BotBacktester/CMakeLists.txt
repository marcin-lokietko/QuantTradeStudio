add_library(BotBacktester STATIC
    BotBacktester.cpp
)

#search for headers in root project directory
target_include_directories(BotBacktester PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_compile_options(BotBacktester PUBLIC ${COMPILE_FLAGS})

target_link_libraries(BotBacktester ${LINKER_FLAGS}
    spdlog

    BotAlgorithms
    Utils
)
