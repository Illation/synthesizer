# files in this library
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.inl)

file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

list (APPEND projectFiles ${headers})
list (APPEND projectFiles ${sources})

precompiled_headers(${sources})
target_definitions()

# library and dependancies
add_library(EtRendering ${sources} ${headers})

assign_source_group(${projectFiles})

libOutputDirectories(EtRendering)

target_link_libraries (EtRendering EtMath EtCore)

# library includes
include_directories("../../dependancies/include/rttr/")
include_directories("../../dependancies/include/gtkmm/")

# general include dirs
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")
target_include_directories (EtRendering PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})