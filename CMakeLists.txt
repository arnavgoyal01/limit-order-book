cmake_minimum_required(VERSION 3.20)
project(OrderBook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # helps clangd / editors

# Opt-in sanitizer build (must come before the targets)
option(ORDERBOOK_SANITIZE "Build with ASan + UBSan" OFF)
if(ORDERBOOK_SANITIZE)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

# The engine: everything except main
add_library(orderbook src/def.cpp src/OrderBook.cpp)
target_include_directories(orderbook PUBLIC src)
target_compile_options(orderbook PRIVATE -Wall -Wextra -Wpedantic)

# The console app
add_executable(ob_cli src/main.cpp)
target_link_libraries(ob_cli PRIVATE orderbook)

# Tests (GoogleTest is downloaded automatically)
include(FetchContent)
FetchContent_Declare(googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(ob_tests tests/test_orderbook.cpp)
target_link_libraries(ob_tests PRIVATE orderbook GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(ob_tests)
