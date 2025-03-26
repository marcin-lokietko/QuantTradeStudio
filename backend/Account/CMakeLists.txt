add_library(Account STATIC
    Account.cpp
)

# include directories
target_include_directories(Account PRIVATE ${PROJECT_SOURCE_DIR})
target_compile_options(Account PUBLIC ${COMPILE_FLAGS})

target_link_libraries(Account ${LINKER_FLAGS}
    glog
)
