

#########################################
# Third party libraries built from source
#########################################


# files in this library
file(GLOB_RECURSE projectFiles ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# library and dependancies
add_library(Vendor ${projectFiles})

assign_source_group(${projectFiles})
libOutputDirectories(Vendor)

# general include dirs
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")
target_include_directories (Vendor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})