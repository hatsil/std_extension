cmake_minimum_required(VERSION 3.2)
project(std_extension CXX)

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
add_library(std_extension STATIC ${SOURCES})

target_include_directories(std_extension PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(std_extension PUBLIC cxx_std_23)

if(USE_BOOST_STACKTRACE)
    add_compile_definitions(USE_BOOST_STACKTRACE)
    include_directories(Boost::stacktrace_backtrace)
    target_link_libraries(std_extension PUBLIC Boost::stacktrace_backtrace)
else()
    target_link_libraries(std_extension PUBLIC -lstdc++exp)
endif()
