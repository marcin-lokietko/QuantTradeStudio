add_library(Config STATIC
    ReadConfig.cpp
    Conversion/ConfigParams.cpp
)

#search for headers in root project directory
target_include_directories(Config PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_compile_options(Config PUBLIC ${COMPILE_FLAGS})

target_link_libraries(Config ${LINKER_FLAGS}
)
