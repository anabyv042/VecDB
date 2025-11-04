cmake_minimum_required(VERSION 3.10)

# Project name
project(VectorDB)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable from your source files
add_executable(vector-db 
    src/main.cpp
    src/storage.cpp
    src/query.cpp
)

# --- GoogleTest setup ---
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2 # (latest stable as of 2025)
)

# Prevent GoogleTest from using old pthread settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

# --- Test executable ---
add_executable(tests
    tests/test_query.cpp
    tests/test_storage.cpp
    src/query.cpp
    src/storage.cpp
)

target_link_libraries(tests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)