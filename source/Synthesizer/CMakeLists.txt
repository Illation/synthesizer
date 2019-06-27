

##############
# Synthesizer
##############


# files
###########
file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.inl)
file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE c_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

list (APPEND projectFiles ${headers})
list (APPEND projectFiles ${sources})
list (APPEND projectFiles ${c_sources})

# setup
#########
precompiled_headers(${sources})
target_definitions()

# rt audio and rt midi defines
if (MSVC)
	add_definitions(-D__WINDOWS_DS__ -D__WINDOWS_ASIO__ -D__WINDOWS_WASAPI__)
	add_definitions(-D__WINDOWS_MM__)
endif(MSVC)

add_definitions(-D_CONSOLE)

# executable and dependancies
add_executable(Synthesizer ${sources} ${c_sources} ${headers})

# directory stuff
assign_source_group(${projectFiles})
outputDirectories(Synthesizer)

add_dependencies(Synthesizer generate-resources)

# linking
target_link_libraries (Synthesizer EtMath EtCore EtRendering Vendor)
dependancyLinks(Synthesizer)

# library includes
include_directories("../../dependancies/include/rttr/")
include_directories("../../dependancies/include/gtkmm/")

# general include dirs
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")
target_include_directories (Synthesizer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

installResources()