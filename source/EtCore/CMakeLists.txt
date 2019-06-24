file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.inl)
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

if (MSVC)
    set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h" )
    foreach( src_file ${sources} )
        set_source_files_properties( ${src_file} PROPERTIES COMPILE_FLAGS "/Yustdafx.h" )
    endforeach( src_file ${sources} )
endif(MSVC)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")

add_library(EtCore ${sources})

target_link_libraries (EtCore EtMath)

target_link_libraries(EtCore rttr)

target_link_libraries(EtCore giomm)

add_definitions(-DWIN32)
add_definitions(-DPLATFORM_Win)
add_definitions(-DPLATFORM_x32)

include_directories("../../dependancies/include/rttr/")
include_directories("../../dependancies/include/gtkmm/")

target_include_directories (EtCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})