file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.inl)

# header only library builds with one source file that includes all headers
add_library(EtMath MathInc.cpp ${headers})

# base directory
target_include_directories (EtMath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

libOutputDirectories(EtMath)