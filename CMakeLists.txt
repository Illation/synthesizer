cmake_minimum_required (VERSION 3.10.0)
project (Synthesizer C CXX)

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

set(ProjectRoot "${CMAKE_BINARY_DIR}/")
set(SourceDir "${ProjectRoot}source/")

# Put built objects all in the same folder so they can be run easily
#set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ProjectRoot}bin/Debug)
#set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ProjectRoot}bin/Release)
#set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${ProjectRoot}lib/Debug)
#set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${ProjectRoot}lib/Release)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory (source)