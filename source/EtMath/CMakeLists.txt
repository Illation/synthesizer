# header only library builds with one source file that includes all headers
add_library(EtMath MathInc.cpp)

# base directory
target_include_directories (EtMath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})