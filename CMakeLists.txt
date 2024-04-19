cmake_minimum_required(VERSION 3.2)
project(stdext CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add debugging information
set(CMAKE_BUILD_TYPE RelWithDebInfo)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

# Create the static library
add_library(stdext STATIC ${SOURCES})

# Link additional libraries
target_link_libraries(stdext PUBLIC -lstdc++_libbacktrace)
