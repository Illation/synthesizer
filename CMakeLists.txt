cmake_minimum_required (VERSION 3.10.0)

# custom build / configuration types

set(CMAKE_CONFIGURATION_TYPES "Debug;Develop;Shipping" CACHE STRING "Available build-types: Debug, Develop and Shipping" FORCE)
set(CMAKE_CXX_FLAGS_DEVELOP "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
set(CMAKE_C_FLAGS_DEVELOP "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
set(CMAKE_CXX_FLAGS_SHIPPING "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_SHIPPING "${CMAKE_C_FLAGS_RELEASE}")

# solution
project (Synthesizer C CXX)

#set_property (GLOBAL PROPERTY USE_FOLDERS ON)

include(build/utils.cmake)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory (source)