# files in this library
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.inl)
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Windows specific settings
if (MSVC)

	# PCH
	#########
	# run this first to set all files to use pch
    foreach( src_file ${sources} )
        set_source_files_properties( ${src_file} PROPERTIES COMPILE_FLAGS "/Yustdafx.h" )
    endforeach( src_file ${sources} )
	# run this second to overwrite the pch setting for the stdafx.cpp file
    set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h" )

	# Platform
	add_definitions(-DPLATFORM_Win)
endif(MSVC)

# architecture
if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
	MESSAGE("++ 64 bit architecture") 
	add_definitions(-DPLATFORM_x64)
 else() 
	MESSAGE("++ 32 bit architecture") 
	add_definitions(-DPLATFORM_x32)
endif()

# library and dependancies
add_library(EtCore ${sources})
target_link_libraries (EtCore EtMath)

# library includes
include_directories("../../dependancies/include/rttr/")
include_directories("../../dependancies/include/gtkmm/")

# general include dirs
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")
target_include_directories (EtCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})