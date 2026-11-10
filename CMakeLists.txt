cmake_minimum_required(VERSION 3.20)
project(lob CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # helps VS Code understand your code

# ---- the library: all your engine code goes here ----
add_library(lob src/order_book.cpp)
target_include_directories(lob PUBLIC include)
target_compile_options(lob PUBLIC -Wall -Wextra -Wpedantic)

# AddressSanitizer + UBSan in Debug builds (catches memory bugs)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(lob PUBLIC -fsanitize=address,undefined -fno-omit-frame-pointer)
  target_link_options(lob PUBLIC -fsanitize=address,undefined)
endif()

# ---- GoogleTest, downloaded automatically ----
include(FetchContent)
FetchContent_Declare(googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(lob_tests tests/test_order_book.cpp)
target_link_libraries(lob_tests PRIVATE lob GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(lob_tests)

# ---- benchmark / replay driver ----
add_executable(replay bench/replay.cpp)
target_link_libraries(replay PRIVATE lob)
