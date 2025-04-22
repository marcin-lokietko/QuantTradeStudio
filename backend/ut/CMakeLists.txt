message(STATUS "### Configuring unit tests...")

find_package(GTest REQUIRED CONFIG)
message(STATUS "Using compilation flags: ${COMPILE_FLAGS}")
message(STATUS "Using linking flags: ${LINKER_FLAGS}")
message(STATUS "GTest version: ${GTest_VERSION}")

# Add test cpp files
add_executable(UnitTests main.cpp ${PROJECT_SOURCE_DIR}/ut/TestDummy.cpp)
target_compile_options(UnitTests PUBLIC ${COMPILE_FLAGS})

# Link test executable against gmock and gtest
target_link_libraries(UnitTests PRIVATE ${LINKER_FLAGS} GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(runUnitTests runUnitTests)
