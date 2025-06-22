add_library(BotAlgorithms STATIC
    ConfigExtractor.cpp
    Rebalancer/Rebalancer.cpp
)

#search for headers in root project directory
target_include_directories(BotAlgorithms PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_compile_options(BotAlgorithms PUBLIC ${COMPILE_FLAGS})

target_link_libraries(BotAlgorithms ${LINKER_FLAGS}
    spdlog

    Utils
)
