cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(GDRemote VERSION 1.0.0)

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    src/Modifiers.cpp
    src/Server.cpp
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

add_compile_definitions(WIN32_LEAN_AND_MEAN=1) # THANK YOU chs000 AND GLOBED

CPMAddPackage("gh:chriskohlhoff/asio#asio-1-32-0")
set(ASIO_INCLUDE_DIR ${asio_SOURCE_DIR}/asio/include)

CPMAddPackage("gh:CrowCpp/Crow@1.2.0")
target_link_libraries(${PROJECT_NAME} Crow)

setup_geode_mod(${PROJECT_NAME})