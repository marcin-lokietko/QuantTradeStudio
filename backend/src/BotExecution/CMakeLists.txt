add_library(BotExecution STATIC
    BotExecution.cpp
    ConfigExtractor.cpp
    Rebalancer/Rebalancer.cpp
)

#search for headers in root project directory
target_include_directories(BotExecution PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_compile_options(BotExecution PUBLIC ${COMPILE_FLAGS})

target_link_libraries(BotExecution ${LINKER_FLAGS}
    glog
)
