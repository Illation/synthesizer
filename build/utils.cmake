

##############################
# general for cmake list files
##############################


# output dir for executables
############################
function(outputDirectories TARGET)

	if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		# 64 bit

		set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/../bin/Debug_x64/${TARGET})
		set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEVELOP ${PROJECT_BINARY_DIR}/../bin/Develop_x64/${TARGET})
		set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_SHIPPING ${PROJECT_BINARY_DIR}/../bin/Shipping_x64/${TARGET})

	else() 
		# 32 bit

		set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/../bin/Debug_x32/${TARGET})
		set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEVELOP ${PROJECT_BINARY_DIR}/../bin/Develop_x32/${TARGET})
		set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_SHIPPING ${PROJECT_BINARY_DIR}/../bin/Shipping_x32/${TARGET})

	endif()

endfunction(outputDirectories)


# output dir for libraries
############################
function(libOutputDirectories TARGET)

	if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		# 64 bit

		set_target_properties(${TARGET} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/../lib/Debug_x64/${TARGET})
		set_target_properties(${TARGET} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEVELOP ${PROJECT_BINARY_DIR}/../lib/Develop_x64/${TARGET})
		set_target_properties(${TARGET} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_SHIPPING ${PROJECT_BINARY_DIR}/../lib/Shipping_x64/${TARGET})

		set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/../lib/Debug_x64/${TARGET})
		set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEVELOP ${PROJECT_BINARY_DIR}/../lib/Develop_x64/${TARGET})
		set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_SHIPPING ${PROJECT_BINARY_DIR}/../lib/Shipping_x64/${TARGET})

	else() 
		# 32 bit

		set_target_properties(${TARGET} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/../lib/Debug_x32/${TARGET})
		set_target_properties(${TARGET} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEVELOP ${PROJECT_BINARY_DIR}/../lib/Develop_x32/${TARGET})
		set_target_properties(${TARGET} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_SHIPPING ${PROJECT_BINARY_DIR}/../lib/Shipping_x32/${TARGET})

		set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/../lib/Debug_x32/${TARGET})
		set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEVELOP ${PROJECT_BINARY_DIR}/../lib/Develop_x32/${TARGET})
		set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_SHIPPING ${PROJECT_BINARY_DIR}/../lib/Shipping_x32/${TARGET})

	endif()

endfunction(libOutputDirectories)


# make project filters mimic directory structure
################################################
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)


# PCH
#########
function(precompiled_headers SOURCELIST)
	# only windows
	if (MSVC)
		# run this first to set all files to use pch
		foreach( src_file ${SOURCELIST} )
			set_source_files_properties( ${src_file} PROPERTIES COMPILE_FLAGS "/Yustdafx.h" )
		endforeach( src_file ${SOURCELIST} )
		# run this second to overwrite the pch setting for the stdafx.cpp file
		set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h" )
	endif(MSVC)
endfunction(precompiled_headers)


# Config defines
#################
function(target_definitions)
	# os
	if (MSVC)
		add_definitions(-DPLATFORM_Win)
	endif(MSVC)
	
	# architecture
	if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		add_definitions(-DPLATFORM_x64)
	 else() 
		add_definitions(-DPLATFORM_x32)
	endif()
endfunction(target_definitions)
