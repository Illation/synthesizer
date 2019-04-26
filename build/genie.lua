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
				targetdir("../bin/" .. cfgs[i] .. "_" .. p[j] .. "/" .. _project)
				--seems like the platform will automatically be added
				objdir("../build/Intermediate/" .. cfgs[i]  .. "/" .. _project)		
		end
	end
	configuration {}
end

--platform specific library paths
function platformLibraries()
	local p = platforms()
	for j = 1, #p do
		local depPf = path.join(DEP_DIR, p[j] .. "/") 

		configuration { "vs*", p[j] }
			libdirs { path.join(depPf, "openAL") }
	end
	configuration {}
end
function staticPlatformLibraries()--libraries built specifically for debug or release
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
				libdirs { }
		end
	end
	configuration {}
end

--copy files that are specific for the platform being built for
function windowsPlatformPostBuild()
	local p = platforms()
	for j = 1, #p do
		local copyCmd = "$(SolutionDir)..\\build\\copyResources_windows.bat " .. path.getabsolute(SOURCE_DIR) .. " $(OutDir) " .. p[j] .. " true"

		configuration { "vs*", p[j] }
			postbuildcommands { copyCmd } --copy dlls and resources after build
	end
	configuration {}
end

configuration "Debug"
	defines { "_DEBUG" }
	flags { "Symbols", "ExtraWarnings" }
configuration "Development"
	flags {"OptimizeSpeed", "Symbols", "ExtraWarnings" }
configuration "Shipping"
	defines { "SHIPPING" }
	flags {"OptimizeSpeed", "No64BitChecks" }

configuration "vs*"
	flags { "NoIncrementalLink", "NoEditAndContinue" }
	linkoptions { "/ignore:4221" }
	defines { "PLATFORM_Win" }
	includedirs { 
		path.join(DEP_INCLUDE, "openAL")
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
		path.join(PROJECT_DIR, "build/*.bat"), 
		path.join(PROJECT_DIR, "build/*.lua"), 
		path.join(PROJECT_DIR, "build/.vahashtags"), --for visual assist
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
		flags { "Winmain"}

	platformLibraries()
	staticPlatformLibraries()
	windowsPlatformPostBuild()

	--Linked libraries
    links{ "openAL" }

	--additional includedirs
	local ProjBase = path.join(SOURCE_DIR, "Synthesizer") 
	includedirs { path.join(ProjBase, "../Synthesizer") }

	--Source files
    files { 
		path.join(SOURCE_DIR, "Synthesizer/**.cpp"), 
		path.join(SOURCE_DIR, "Synthesizer/**.hpp"), 
		path.join(SOURCE_DIR, "Synthesizer/**.h"), 
		path.join(SOURCE_DIR, "Synthesizer/**.inl"), 
	}

	nopch { 	}	--c code shouldn't use precompiled headers

	pchheader "stdafx.h"
	pchsource "../source/Synthesizer/stdafx.cpp"
