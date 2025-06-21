add_library(Wallet STATIC
    Wallet.cpp
)

#search for headers in root project directory
target_include_directories(Wallet PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_compile_options(Wallet PUBLIC ${COMPILE_FLAGS})

target_link_libraries(Wallet ${LINKER_FLAGS}
    fmt::fmt
    spdlog
)
