

##############################
# general for cmake list files
##############################


# output dir for executables
############################
function(outputDirectories TARGET)

	if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		# 64 bit

		set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/../bin/Debug_x64/bin)
		set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEVELOP ${PROJECT_BINARY_DIR}/../bin/Develop_x64/bin)
		set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_SHIPPING ${PROJECT_BINARY_DIR}/../bin/Shipping_x64/bin)

	else() 
		# 32 bit

		set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/../bin/Debug_x32/bin)
		set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEVELOP ${PROJECT_BINARY_DIR}/../bin/Develop_x32/bin)
		set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_SHIPPING ${PROJECT_BINARY_DIR}/../bin/Shipping_x32/bin)

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

	message(STATUS "library directory: ${dep_pf}")

	set(gtk_dbg "${dep_pf}/gtkmm/Debug/")
	set(gtk_rel "${dep_pf}/gtkmm/Release/")

	# separate debug and release libs
	target_link_libraries (${TARGET} 		
		debug ${dep_pf}/rttr/Debug/rttr_core_d.lib		optimized ${dep_pf}/rttr/Release/rttr_core.lib # RTTR

		debug ${gtk_dbg}bz2d.lib				optimized ${gtk_rel}bz2.lib			# GTK
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
		debug ${gtk_dbg}atkmm.lib				optimized ${gtk_rel}atkmm.lib	 
		debug ${gtk_dbg}cairomm-1.0.lib			optimized ${gtk_rel}cairomm-1.0.lib	 
		debug ${gtk_dbg}epoxy.lib				optimized ${gtk_rel}epoxy.lib	 
		debug ${gtk_dbg}expat.lib				optimized ${gtk_rel}expat.lib	 
		debug ${gtk_dbg}fontconfig.lib			optimized ${gtk_rel}fontconfig.lib	 
		debug ${gtk_dbg}gailutil-3.0.lib		optimized ${gtk_rel}gailutil-3.0.lib	 
		debug ${gtk_dbg}gdk_pixbuf-2.0.lib		optimized ${gtk_rel}gdk_pixbuf-2.0.lib	 
		debug ${gtk_dbg}gdk-3.0.lib				optimized ${gtk_rel}gdk-3.0.lib	 
		debug ${gtk_dbg}gdkmm.lib				optimized ${gtk_rel}gdkmm.lib	 
		debug ${gtk_dbg}gio-2.0.lib				optimized ${gtk_rel}gio-2.0.lib	 
		debug ${gtk_dbg}giomm.lib				optimized ${gtk_rel}giomm.lib	 
		debug ${gtk_dbg}glib-2.0.lib			optimized ${gtk_rel}glib-2.0.lib	 
		debug ${gtk_dbg}glibmm.lib				optimized ${gtk_rel}glibmm.lib	 
		debug ${gtk_dbg}gmodule-2.0.lib			optimized ${gtk_rel}gmodule-2.0.lib	 
		debug ${gtk_dbg}gobject-2.0.lib			optimized ${gtk_rel}gobject-2.0.lib	 
		debug ${gtk_dbg}gthread-2.0.lib			optimized ${gtk_rel}gthread-2.0.lib	 
		debug ${gtk_dbg}gtk-3.0.lib				optimized ${gtk_rel}gtk-3.0.lib	 
		debug ${gtk_dbg}gtkmm.lib				optimized ${gtk_rel}gtkmm.lib	 
		debug ${gtk_dbg}harfbuzz.lib			optimized ${gtk_rel}harfbuzz.lib	 
		debug ${gtk_dbg}libcharset.lib			optimized ${gtk_rel}libcharset.lib	 
		debug ${gtk_dbg}libffi.lib				optimized ${gtk_rel}libffi.lib	 
		debug ${gtk_dbg}libiconv.lib			optimized ${gtk_rel}libiconv.lib	 
		debug ${gtk_dbg}libintl.lib				optimized ${gtk_rel}libintl.lib	 
		debug ${gtk_dbg}pango-1.0.lib			optimized ${gtk_rel}pango-1.0.lib	 
		debug ${gtk_dbg}pangocairo-1.0.lib		optimized ${gtk_rel}pangocairo-1.0.lib	 
		debug ${gtk_dbg}pangoft2-1.0.lib		optimized ${gtk_rel}pangoft2-1.0.lib	 
		debug ${gtk_dbg}pangomm.lib				optimized ${gtk_rel}pangomm.lib	 
		debug ${gtk_dbg}pangowin32-1.0.lib		optimized ${gtk_rel}pangowin32-1.0.lib	 
		debug ${gtk_dbg}sigc-2.0.lib			optimized ${gtk_rel}sigc-2.0.lib	 )

endfunction(dependancyLinks)


# install everything in the appropriate directory according to configuration
###########################
function(installResources)

	set(projectBase "${PROJECT_BINARY_DIR}/..")
	set(baseBinDir "${projectBase}/bin")
	set(packagedResDir "${projectBase}/resources/packaged")
	set(configDir "${projectBase}/resources/config")

	# user directory where user data and configuration
	set(userDir "${projectBase}/user_data")

	# create a file within the packaged resources that points to the user directory
	get_filename_component(absUserDir ${userDir} ABSOLUTE)
	file(WRITE ${packagedResDir}/config/userDirPointer.json
		"{\"dir pointer\":{\"user dir path\":\"${absUserDir}/\"}}" )

	# copy config files
	install(DIRECTORY ${configDir}/ DESTINATION ${userDir}/)

	# paths for our libraries depend on the architecture we compile fo
	if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		set(platform "_x64")
		set(rttrDir "${projectBase}/dependancies/x64/rttr")
		set(gtkDir "${projectBase}/dependancies/x64/gtkmm")
	 else() 
		set(platform "_x32")
		set(rttrDir "${projectBase}/dependancies/x32/rttr")
		set(gtkDir "${projectBase}/dependancies/x32/gtkmm")
	endif()

	foreach(configType ${CMAKE_CONFIGURATION_TYPES})

		# where the lib files live
		set(libcfg "Release") 
		if("${configType}" STREQUAL "Debug" )
			set(libcfg "Debug")
		endif()

		# copy dlls for all libraries
		install(DIRECTORY ${rttrDir}/${libcfg}/
			CONFIGURATIONS ${configType}
			DESTINATION ${baseBinDir}/${configType}${platform}/bin/
			FILES_MATCHING PATTERN "*.dll")

		install(DIRECTORY ${gtkDir}/${libcfg}/
			CONFIGURATIONS ${configType}
			DESTINATION ${baseBinDir}/${configType}${platform}/bin/
			FILES_MATCHING PATTERN "*.dll")

		# for debug applications we also copy pdbs
		if("${configType}" STREQUAL "Debug" )
			install(DIRECTORY ${rttrDir}/${libcfg}/
				CONFIGURATIONS ${configType}
				DESTINATION ${baseBinDir}/${configType}${platform}/bin/
				FILES_MATCHING PATTERN "*.pdb")

			install(DIRECTORY ${gtkDir}/${libcfg}/
				CONFIGURATIONS ${configType}
				DESTINATION ${baseBinDir}/${configType}${platform}/bin/
				FILES_MATCHING PATTERN "*.pdb")
		endif()

		# copy packaged resources
		install(DIRECTORY ${packagedResDir}/
			CONFIGURATIONS ${configType}
			DESTINATION ${baseBinDir}/${configType}${platform}/ )

	endforeach()

endfunction(installResources)
