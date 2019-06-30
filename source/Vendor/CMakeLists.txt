

#########################################
# Third party libraries built from source
#########################################


# files in this library
file(GLOB_RECURSE projectFiles ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# rt audio and rt midi defines
if (MSVC)
	add_definitions(-D__WINDOWS_DS__ -D__WINDOWS_ASIO__ -D__WINDOWS_WASAPI__)
	add_definitions(-D__WINDOWS_MM__)
endif(MSVC)

# library and dependancies
add_library(Vendor ${projectFiles})

assign_source_group(${projectFiles})
libOutputDirectories(Vendor)

# general include dirs
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")
target_include_directories (Vendor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})