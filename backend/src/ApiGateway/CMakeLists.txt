add_library(ApiGateway STATIC
    ApiGateway.cpp
)

#search for headers in root project directory
target_include_directories(ApiGateway PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_compile_options(ApiGateway PUBLIC ${COMPILE_FLAGS})

target_link_libraries(ApiGateway ${LINKER_FLAGS}
    spdlog
)
