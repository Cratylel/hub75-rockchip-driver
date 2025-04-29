cmake_minimum_required(VERSION 3.14)
project(MatrixMainExecutable LANGUAGES CXX)

# Set C++ standard and flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -g -rdynamic")

# Optional preprocessor defines
if(TEST_ENV)
    add_compile_definitions(TEST_ENV)
endif()
if(LOG_LEVEL_DEBUG)
    add_compile_definitions(LOG_LEVEL_DEBUG)
endif()
if(TEST_SERVER)
    add_compile_definitions(TEST_SERVER)
endif()

# Include paths
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/lib)

# Get all .cpp files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/lib/*.cpp"
)

# Detect architecture
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" ARCH_STR)

# Normalize common values
if(ARCH_STR MATCHES "x86_64|amd64")
    set(ARCH_SUFFIX "x86")
elseif(ARCH_STR MATCHES "arm*")
    set(ARCH_SUFFIX "arm")
elseif(ARCH_STR MATCHES "aarch64")
    set(ARCH_SUFFIX "arm64")
else()
    set(ARCH_SUFFIX "${ARCH_STR}")
endif()

# Final executable
set(EXECUTABLE_NAME "main_${ARCH_SUFFIX}")
add_executable(${EXECUTABLE_NAME} ${SOURCES})
