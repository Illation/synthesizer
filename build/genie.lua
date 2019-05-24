solution "Synthesizer"
    configurations {
        "Debug",
        "Development",
		"Shipping"
    }

    platforms {
        "x64",
        "x32"
    }

    language "C++"

    location "../build/"
    objdir "../build/"
	
PROJECT_DIR = "../"
SOURCE_DIR = path.join(PROJECT_DIR, "source/")
DEP_DIR = path.join(PROJECT_DIR, "dependancies/")
DEP_INCLUDE = path.join(DEP_DIR, "include/")

--setting output directories
--intermediate files go in		build/Intermediate/config_platform/project
--binaries go in				bin/config/project/platform
function outputDirectories(_project)
	local cfgs = configurations()
	local p = platforms()
	for i = 1, #cfgs do
		for j = 1, #p do
			configuration { cfgs[i], p[j] }
				targetdir("../bin/" .. cfgs[i] .. "_" .. p[j] .. "/" .. "bin")
				--seems like the platform will automatically be added
				objdir("../synthesizer/build/Intermediate/" .. cfgs[i]  .. "/" .. _project)		
		end
	end
	configuration {}
end

function libOutputDirectories(_library)
	local cfgs = configurations()
	local p = platforms()
	for i = 1, #cfgs do
		for j = 1, #p do
			configuration { cfgs[i], p[j] }
				targetdir("../lib/" .. cfgs[i] .. "_" .. p[j] .. "/" .. _library)
				objdir("../synthesizer/build/Intermediate/" .. cfgs[i]  .. "/" .. _library)		--seems like the platform will automatically be added
		end
	end
	configuration {}
end

--platform specific library paths
function platformLibraries()--libraries built specifically for debug or release
	local cfgs = configurations()
	local p = platforms()
	for i = 1, #cfgs do
		for j = 1, #p do
			local depPf = path.join(DEP_DIR, p[j] .. "/") 
			local suffix = ""
			if(string.startswith(cfgs[i], "Debug"))
				then suffix = "/Debug"
				else suffix = "/Release"
			end

			configuration { "vs*", p[j], cfgs[i] }
				libdirs { 
					path.join(depPf, "gtkmm" .. suffix), 
					path.join(depPf, "rttr" .. suffix) 
				}
		end
	end
	configuration {}
end

--copy files that are specific for the platform being built for
function windowsPlatformExtraBuildSteps()
	local p = platforms()
	for j = 1, #p do
		local copyCmd = "call $(SolutionDir)..\\build\\copyResources_windows.bat " .. path.getabsolute(SOURCE_DIR) .. " $(OutDir) " .. p[j] .. " true $(Configuration)"
		local schemaCmd = "call $(SolutionDir)..\\build\\compileSchemas_windows.bat " .. path.getabsolute(SOURCE_DIR) .. " $(OutDir) "

		local compileCmd = "$(SolutionDir)..\\build\\compileResources_windows.bat " .. path.getabsolute(SOURCE_DIR)
		configuration { "vs*", p[j] }
			prebuildcommands { compileCmd }
			postbuildcommands { schemaCmd .. ";\n", copyCmd } --copy dlls and resources after build
	end
	configuration {}
end

function dependancyLinks()
	--Linked libraries with varying names per configuration
	configuration "Debug"
		-- RTTR
		links { "rttr_core_d" }
		-- GTKmm
		links { "bz2d", "cairod", "cairo-gobjectd", "freetyped", "libpng16d", "pcre16d", "pcre32d", "pcrecppd", "pcred", "pcreposixd", "pixman-1d", "zlibd" }
	configuration "Development"
		-- RTTR
		links { "rttr_core" }
		-- GTKmm
		links { "bz2", "cairo", "cairo-gobject", "freetype", "libpng16", "pcre16", "pcre32", "pcrecpp", "pcre", "pcreposix", "pixman-1", "zlib" }
	configuration "Shipping"
		-- RTTR
		links { "rttr_core" }
		-- GTKmm
		links { "bz2", "cairo", "cairo-gobject", "freetype", "libpng16", "pcre16", "pcre32", "pcrecpp", "pcre", "pcreposix", "pixman-1", "zlib" }
	configuration {}

    links{ 
		--GTK
		"atk-1.0" 
		, "atkmm" 
		, "cairomm-1.0" 
		, "epoxy" 
		, "expat" 
		, "fontconfig" 
		, "gailutil-3.0" 
		, "gdk_pixbuf-2.0" 
		, "gdk-3.0" 
		, "gdkmm" 
		, "gio-2.0" 
		, "giomm" 
		, "glib-2.0" 
		, "glibmm" 
		, "gmodule-2.0" 
		, "gobject-2.0" 
		, "gthread-2.0" 
		, "gtk-3.0" 
		, "gtkmm" 
		, "harfbuzz" 
		, "libcharset" 
		, "libffi" 
		, "libiconv" 
		, "libintl" 
		, "pango-1.0" 
		, "pangocairo-1.0" 
		, "pangoft2-1.0" 
		, "pangomm" 
		, "pangowin32-1.0" 
		, "sigc-2.0" 
	}
end

configuration "Debug"
	defines { "_DEBUG", "__RTMIDI_DEBUG__ " }
	flags { "Symbols", "ExtraWarnings" }
configuration "Development"
	flags {"OptimizeSpeed", "Symbols", "ExtraWarnings" }
configuration "Shipping"
	defines { "SHIPPING" }
	flags {"OptimizeSpeed", "No64BitChecks" }

configuration "vs*"
	flags { "NoIncrementalLink", "NoEditAndContinue" }
	linkoptions { "/ignore:4221" }
	defines { 
		"PLATFORM_Win", 
		"__WINDOWS_DS__", "__WINDOWS_ASIO__", "__WINDOWS_WASAPI__", -- RtAudio
		"__WINDOWS_MM__"											-- RtMidi
		}
	includedirs { 
		path.join(DEP_INCLUDE, "gtkmm"),
		path.join(DEP_INCLUDE, "rttr")
	}
	debugdir "$(OutDir)"
configuration { "vs*", "x32" }
	defines { "WIN32" }
configuration { "x32" }
	defines { "PLATFORM_x32" }
configuration { "vs*", "x64" }
	defines { "WIN64" }
configuration { "x64" }
	defines { "PLATFORM_x64" }
configuration { "linux", "gmake"}
	defines { "PLATFORM_Linux", "__linux__" }
	includedirs { "/usr/include" }
	buildoptions_cpp { "-std=c++14" }
configuration {}

startproject "Synthesizer"

--project that has the build files, easy way to make sure we can see the build files in the IDE, shouldn't actually be built
project "General"
	kind "StaticLib" --required
	location "."
	--specific files to avoid showing vs projects and solutions in build folder
	files { 
		path.join(PROJECT_DIR, "resources/**.ui"), 
		path.join(PROJECT_DIR, "resources/**.gresource.xml"), 
		path.join(PROJECT_DIR, "resources/**.gschema.xml"), 
		path.join(PROJECT_DIR, "build/*.bat"), 
		path.join(PROJECT_DIR, "build/*.lua"), 
		path.join(PROJECT_DIR, "build/.vahashtags"), --for visual assist
		path.join(PROJECT_DIR, "doc/*.*"), 
		path.join(PROJECT_DIR, "*.*"), 
		path.join(PROJECT_DIR, ".gitignore"), 
		path.join(PROJECT_DIR, ".gitattributes") 
	}

--in future this should be generated for any application running on the engine
project "Synthesizer"
	kind "ConsoleApp"

	location "../source/Synthesizer"

    defines { "_CONSOLE" }

	--Set output folders
	outputDirectories("Synthesizer")

	configuration "vs*"
		flags { "Winmain" }
		links { 
			"dsound", --rtAudio 
			"winmm" -- rtMidi on windows
		} 

	platformLibraries()
	windowsPlatformExtraBuildSteps()

	dependancyLinks()

	--Internal static links
    links{ 
		"Vendor"
		, "EtCore" 
		, "EtMath" 
	}

	--additional includedirs
	local ProjBase = path.join(SOURCE_DIR, "Synthesizer") 
	includedirs { SOURCE_DIR, ProjBase }

	--Source files
    files { 
		path.join(SOURCE_DIR, "Synthesizer/**.cpp"), 
		path.join(SOURCE_DIR, "Synthesizer/**.hpp"), 
		path.join(SOURCE_DIR, "Synthesizer/**.h"), 
		path.join(SOURCE_DIR, "Synthesizer/**.inl"), 
		
		path.join(SOURCE_DIR, "Synthesizer/UI/_generated/resources.c"), 
	}

	nopch { 	
		path.join(SOURCE_DIR, "Synthesizer/UI/_generated/resources.c"),  
	}	--generated code shouldn't use precompiled headers

	pchheader "stdafx.h"
	pchsource "../source/Synthesizer/stdafx.cpp"


-- Math library from E.T.Engine
project "EtMath"
    kind "StaticLib"

	location "../source/EtMath"
	
	libOutputDirectories("EtMath")

	--additional includedirs
	local ProjBase = path.join(SOURCE_DIR, "EtMath") 
	includedirs { ProjBase }

    files { 
		path.join(SOURCE_DIR, "EtMath/**.cpp"), 
		path.join(SOURCE_DIR, "EtMath/**.inl"), 
		path.join(SOURCE_DIR, "EtMath/**.h"), 
	}

	--additional includedirs
	includedirs { SOURCE_DIR }


-- Base framework from E.T.Engine
project "EtCore"
    kind "StaticLib"

	location "../source/EtCore"
	
	libOutputDirectories("EtCore")

	--additional includedirs
	local ProjBase = path.join(SOURCE_DIR, "EtCore") 
	includedirs { ProjBase }

    files { 
		path.join(SOURCE_DIR, "EtCore/**.cpp"), 
		path.join(SOURCE_DIR, "EtCore/**.hpp"), 
		path.join(SOURCE_DIR, "EtCore/**.inl"), 
		path.join(SOURCE_DIR, "EtCore/**.h"), 
	}

	--additional includedirs
	includedirs { SOURCE_DIR }

	pchheader "stdafx.h"
	pchsource "../source/EtCore/stdafx.cpp"


-- all non prebuilt vendor libraries go here
project "Vendor"
    kind "StaticLib"

	location "../source/Vendor"
	
	libOutputDirectories("Vendor")

    files { 
		path.join(SOURCE_DIR, "Vendor/**.cpp"), 
		path.join(SOURCE_DIR, "Vendor/**.h"), 
	}

	--additional includedirs
	local ProjBase = path.join(SOURCE_DIR, "Vendor") 
	includedirs { ProjBase }
