

#####################
# Resource CMake file
#####################


# resources generating compiled source files
############################################
set(gen_dir "${CMAKE_CURRENT_SOURCE_DIR}/../source/Synthesizer/UI/_generated/")
set(cmp_dir "${CMAKE_CURRENT_SOURCE_DIR}/compiled/")
set(tool_dir "${CMAKE_CURRENT_SOURCE_DIR}/../dependancies/tools/")

set(target_file "${gen_dir}resources.c")
set(res_file "${cmp_dir}synthesizer.gresource.xml")

set(xml_env "${CMAKE_COMMAND} -E env \"XMLLINT=${tool_dir}xmllint/xmllint.exe\"")

# any files that can trigger the resources to be rebuilt
file(GLOB_RECURSE deps ${CMAKE_CURRENT_SOURCE_DIR}/compiled/*.*)

# the command list that will run - create dir, set env var, run compile source, log stuff
add_custom_command(
    OUTPUT ${target_file}
    DEPENDS ${deps}

	COMMAND ${CMAKE_COMMAND} -E echo "Prebuild: Generating compiled glib sources - Source Directory: ${cmp_dir} ; Generated Directory: ${gen_dir}"
	COMMAND ${CMAKE_COMMAND} -E make_directory ${gen_dir}
    COMMAND ${tool_dir}glib-compile-resources.exe --manual-register --target=${target_file} --sourcedir=${cmp_dir} --generate-source ${res_file}
	COMMAND ${CMAKE_COMMAND} -E echo ""
	COMMAND ${CMAKE_COMMAND} -E echo ""
	
    COMMENT "Generating resource source file for GLib"

    VERBATIM
)

add_custom_target( generate-resources DEPENDS ${target_file} )

set_source_files_properties( ${target_file} PROPERTIES GENERATED TRUE )


# resources generating compiled source files
############################################

# we place our generated files in the packaged folder which will get copied to the final build folder later
set(schema_out "${CMAKE_CURRENT_SOURCE_DIR}/packaged/share/glib-2.0/schemas/") 
set(schema_dir "${CMAKE_CURRENT_SOURCE_DIR}/compiled/ui/schemas/")

set(schema_file "${schema_dir}gschemas.compiled")

file(GLOB_RECURSE schema_deps ${schema_dir}*.*)

add_custom_command(
    OUTPUT ${schema_file}
    DEPENDS ${schema_deps}

	COMMAND ${CMAKE_COMMAND} -E echo "--"
	COMMAND ${CMAKE_COMMAND} -E echo "--"
	COMMAND ${CMAKE_COMMAND} -E echo "PostBuild: compiling schema files - Source Directory: ${schema_dir} ; Output Directory: ${schema_out} ;"
	COMMAND ${CMAKE_COMMAND} -E make_directory ${schema_out}
    COMMAND ${tool_dir}glib-compile-schemas.exe --targetdir=${schema_out} ${schema_dir}
	COMMAND ${CMAKE_COMMAND} -E echo "--"
	COMMAND ${CMAKE_COMMAND} -E echo "--"
	
    COMMENT "Generating schema file for glib"

    VERBATIM
)

add_custom_target( compile-schemas ALL DEPENDS ${schema_file} )
