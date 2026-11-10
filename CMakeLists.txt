cmake_minimum_required(VERSION 3.25)
project(lob CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # helps VS Code / clangd understand your code

# ---- warnings: applied ONLY to our own targets (never to GoogleTest) ----
option(LOB_WARNINGS_AS_ERRORS "Treat compiler warnings as errors for project targets" ON)

add_library(lob_warnings INTERFACE)
target_compile_options(lob_warnings INTERFACE -Wall -Wextra -Wpedantic)
if(LOB_WARNINGS_AS_ERRORS)
  target_compile_options(lob_warnings INTERFACE -Werror)
endif()

# ---- GoogleTest, downloaded automatically ----
# SYSTEM => its headers are included with -isystem, so warnings inside
# gtest headers can't break our -Werror test builds.
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
include(FetchContent)
FetchContent_Declare(googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
  SYSTEM)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory(src)
add_subdirectory(app)
add_subdirectory(tests)
add_subdirectory(benchmarks)
