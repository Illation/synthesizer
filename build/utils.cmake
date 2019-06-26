

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


# link to all dependancies
###########################
function(dependancyLinks TARGET)

	set(dep_dir "${PROJECT_BINARY_DIR}/../dependancies")

	if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		set(dep_pf "${dep_dir}/x64")
	 else() 
		set(dep_pf "${dep_dir}/x32")
	endif()

	set(gtk_dbg "${dep_pf}/gtkmm/Debug/")
	set(gtk_rel "${dep_pf}/gtkmm/Release/")

	# separate debug and release libs
	target_link_libraries (${TARGET} 		
		debug ${dep_pf}/rttr/Debug/rttr_core_d.lib		optimized ${dep_pf}/rttr/Release/rttr_core.lib # RTTR

		debug ${gtk_dbg}bz2d.lib				optimized ${gtk_rel}bbz2.lib			# GTK
		debug ${gtk_dbg}cairod.lib				optimized ${gtk_rel}cairo.lib	
		debug ${gtk_dbg}cairo-gobjectd.lib		optimized ${gtk_rel}cairo-gobject.lib	
		debug ${gtk_dbg}freetyped.lib			optimized ${gtk_rel}freetype.lib	
		debug ${gtk_dbg}libpng16d.lib			optimized ${gtk_rel}libpng16.lib	
		debug ${gtk_dbg}pcre16d.lib				optimized ${gtk_rel}pcre16.lib	
		debug ${gtk_dbg}pcre32d.lib				optimized ${gtk_rel}pcre32.lib	
		debug ${gtk_dbg}pcrecppd.lib			optimized ${gtk_rel}pcrecpp.lib	
		debug ${gtk_dbg}pcred.lib				optimized ${gtk_rel}pcre.lib	
		debug ${gtk_dbg}pcreposixd.lib			optimized ${gtk_rel}pcreposix.lib	
		debug ${gtk_dbg}pixman-1d.lib			optimized ${gtk_rel}pixman-1.lib	
		debug ${gtk_dbg}zlibd.lib				optimized ${gtk_rel}zlib.lib	

		debug ${gtk_dbg}atk-1.0.lib				optimized ${gtk_rel}atk-1.0.lib	 
		debug ${gtk_dbg}aatkmm.lib				optimized ${gtk_rel}aatkmm.lib	 
		debug ${gtk_dbg}acairomm-1.0.lib		optimized ${gtk_rel}acairomm-1.0.lib	 
		debug ${gtk_dbg}aepoxy.lib				optimized ${gtk_rel}aepoxy.lib	 
		debug ${gtk_dbg}aexpat.lib				optimized ${gtk_rel}aexpat.lib	 
		debug ${gtk_dbg}afontconfig.lib			optimized ${gtk_rel}afontconfig.lib	 
		debug ${gtk_dbg}agailutil-3.0.lib		optimized ${gtk_rel}agailutil-3.0.lib	 
		debug ${gtk_dbg}agdk_pixbuf-2.0.lib		optimized ${gtk_rel}agdk_pixbuf-2.0.lib	 
		debug ${gtk_dbg}agdk-3.0.lib			optimized ${gtk_rel}agdk-3.0.lib	 
		debug ${gtk_dbg}agdkmm.lib				optimized ${gtk_rel}agdkmm.lib	 
		debug ${gtk_dbg}agio-2.0.lib			optimized ${gtk_rel}agio-2.0.lib	 
		debug ${gtk_dbg}agiomm.lib				optimized ${gtk_rel}agiomm.lib	 
		debug ${gtk_dbg}aglib-2.0.lib			optimized ${gtk_rel}aglib-2.0.lib	 
		debug ${gtk_dbg}aglibmm.lib				optimized ${gtk_rel}aglibmm.lib	 
		debug ${gtk_dbg}agmodule-2.0.lib		optimized ${gtk_rel}agmodule-2.0.lib	 
		debug ${gtk_dbg}agobject-2.0.lib		optimized ${gtk_rel}agobject-2.0.lib	 
		debug ${gtk_dbg}agthread-2.0.lib		optimized ${gtk_rel}agthread-2.0.lib	 
		debug ${gtk_dbg}agtk-3.0.lib			optimized ${gtk_rel}agtk-3.0.lib	 
		debug ${gtk_dbg}agtkmm.lib				optimized ${gtk_rel}agtkmm.lib	 
		debug ${gtk_dbg}aharfbuzz.lib			optimized ${gtk_rel}aharfbuzz.lib	 
		debug ${gtk_dbg}alibcharset.lib			optimized ${gtk_rel}alibcharset.lib	 
		debug ${gtk_dbg}alibffi.lib				optimized ${gtk_rel}alibffi.lib	 
		debug ${gtk_dbg}alibiconv.lib			optimized ${gtk_rel}alibiconv.lib	 
		debug ${gtk_dbg}alibintl.lib			optimized ${gtk_rel}alibintl.lib	 
		debug ${gtk_dbg}apango-1.0.lib			optimized ${gtk_rel}apango-1.0.lib	 
		debug ${gtk_dbg}apangocairo-1.0.lib		optimized ${gtk_rel}apangocairo-1.0.lib	 
		debug ${gtk_dbg}apangoft2-1.0.lib		optimized ${gtk_rel}apangoft2-1.0.lib	 
		debug ${gtk_dbg}apangomm.lib			optimized ${gtk_rel}apangomm.lib	 
		debug ${gtk_dbg}apangowin32-1.0.lib		optimized ${gtk_rel}apangowin32-1.0.lib	 
		debug ${gtk_dbg}asigc-2.0.lib			optimized ${gtk_rel}asigc-2.0.lib	 )

endfunction(dependancyLinks)
