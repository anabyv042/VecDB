cmake_minimum_required(VERSION 3.28)

# Project name
project(VectorDB)

# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(vector_db_lib
  src/storage.cpp
  src/query.cpp
  src/storage/page.cpp
)

add_executable(vector-db
  src/main.cpp
)
target_link_libraries(vector-db PRIVATE vector_db_lib)

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
  tests/storage/test_page.cpp
)
target_link_libraries(tests PRIVATE vector_db_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)