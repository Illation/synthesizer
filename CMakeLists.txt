

#####################
# Main CMake file
#####################


# CMake version
cmake_minimum_required (VERSION 3.10.0)

# utility functions
include(build/utils.cmake)

# custom build / configuration types
####################################

set(CMAKE_CONFIGURATION_TYPES "Debug;Develop;Shipping" CACHE STRING "Available build-types: Debug, Develop and Shipping" FORCE)

# copy settings from existing build types
set(CMAKE_CXX_FLAGS_DEVELOP "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
set(CMAKE_C_FLAGS_DEVELOP "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
set(CMAKE_EXE_LINKER_FLAGS_DEVELOP "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")
set(CMAKE_CXX_FLAGS_SHIPPING "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_SHIPPING "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_EXE_LINKER_FLAGS_SHIPPING "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")

# solution
##########

project (Synthesizer C CXX) # C++
set(CMAKE_CXX_STANDARD 14)  # C++ 14

# general library that has the build files, easy way to make sure we can see the build files in the IDE, shouldn't actually be built
#####################################

file(GLOB_RECURSE generalFiles 
	${CMAKE_CURRENT_SOURCE_DIR}/resources/*.ui 
	${CMAKE_CURRENT_SOURCE_DIR}/resources/*.json 
	${CMAKE_CURRENT_SOURCE_DIR}/resources/*.glsl 
	${CMAKE_CURRENT_SOURCE_DIR}/resources/*.gresource.xml 
	${CMAKE_CURRENT_SOURCE_DIR}/resources/*.gschema.xml
	${CMAKE_CURRENT_SOURCE_DIR}/build/*.bat
	${CMAKE_CURRENT_SOURCE_DIR}/build/*.lua
	${CMAKE_CURRENT_SOURCE_DIR}/build/*.cmake
	${CMAKE_CURRENT_SOURCE_DIR}/build/.vahashtags # visual assist
	${CMAKE_CURRENT_SOURCE_DIR}/doc/*.*
	${CMAKE_CURRENT_SOURCE_DIR}/source/CMakeLists.txt
	${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
	${CMAKE_CURRENT_SOURCE_DIR}/README.md
	${CMAKE_CURRENT_SOURCE_DIR}/.gitignore
	${CMAKE_CURRENT_SOURCE_DIR}/.gitattributes )

add_library(General ${generalFiles})
set_target_properties(General PROPERTIES LINKER_LANGUAGE NONE) # we don't build this library
assign_source_group(${generalFiles})

# all other targets are under source and resources
##################################################

add_subdirectory (resources)
add_subdirectory (source)