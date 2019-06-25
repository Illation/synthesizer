# general for cmake list files
##############################

# output dir for executables
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