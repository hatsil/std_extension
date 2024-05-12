cmake_minimum_required(VERSION 3.28)
project(std_extention CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add debugging information
set(CMAKE_BUILD_TYPE RelWithDebInfo)

file(GLOB SOURCES "src/*.cpp")

# Create the static library
add_library(std_extention STATIC ${SOURCES})

target_include_directories(std_extention PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(std_extention PUBLIC cxx_std_23)
# Link additional libraries
target_link_libraries(std_extention PUBLIC -lstdc++_libbacktrace)
