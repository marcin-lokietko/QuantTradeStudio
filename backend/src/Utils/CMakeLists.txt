add_library(Utils STATIC
    Time/SystemTime.cpp
)

#search for headers in root project directory
target_include_directories(Utils PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_compile_options(Utils PUBLIC ${COMPILE_FLAGS})

target_link_libraries(Utils ${LINKER_FLAGS}
    spdlog
)
