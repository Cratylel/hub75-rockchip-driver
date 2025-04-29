cmake_minimum_required(VERSION 3.10)
project(GPIOTests)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Add the GPIO source files
add_library(GPIOLib lib/gpio.cpp)
target_include_directories(GPIOLib PUBLIC lib)

# Add test executable
add_executable(test_gpio test/test_gpio.cpp)
target_link_libraries(test_gpio GPIOLib gtest gtest_main)

# Enable CTest and add a test
add_test(NAME GPIOTests COMMAND test_gpio)
