# files in this library
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.inl)

list (APPEND projectFiles ${headers})
list (APPEND projectFiles MathInc.cpp)

target_definitions()

# header only library builds with one source file that includes all headers
add_library(EtMath ${projectFiles})

assign_source_group(${projectFiles})

# base directory
target_include_directories (EtMath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

libOutputDirectories(EtMath)