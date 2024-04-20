cmake_minimum_required(VERSION 3.2)
project(stdext CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add debugging information
set(CMAKE_BUILD_TYPE RelWithDebInfo)

file(GLOB SOURCES "src/*.cpp")

# Create the static library
add_library(stdext STATIC ${SOURCES})

target_include_directories(stdext PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(stdext PUBLIC cxx_std_23)
# Link additional libraries
target_link_libraries(stdext PUBLIC -lstdc++_libbacktrace)
