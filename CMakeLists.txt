cmake_minimum_required(VERSION 3.2)
project(std_extention CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if USE_BOOST_STACKTRACE environment variable is set
if(DEFINED ENV{USE_BOOST_STACKTRACE})
    set(USE_BOOST_STACKTRACE ON)
else()
    set(USE_BOOST_STACKTRACE OFF)
endif()

# Add debugging information
set(CMAKE_BUILD_TYPE RelWithDebInfo)

if(USE_BOOST_STACKTRACE)
    include(cmake/boost_stacktrace.cmake)
endif()

file(GLOB SOURCES "src/*.cpp")

# Create the static library
add_library(std_extention STATIC ${SOURCES})

target_include_directories(std_extention PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(std_extention PUBLIC cxx_std_23)

if(USE_BOOST_STACKTRACE)
    target_compile_definitions(std_extention PUBLIC USE_BOOST_STACKTRACE)
    target_link_libraries(std_extention PUBLIC Boost::stacktrace_backtrace)
else()
    target_link_libraries(std_extention PUBLIC -lstdc++exp)
endif()
