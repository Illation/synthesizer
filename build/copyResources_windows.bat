@ECHO OFF

SET SOURCE_DIR=%1
SET OUT_DIR=%2
IF [%3] == [] (
	SET PLATFORM=x32
) ELSE (
	SET PLATFORM=%3
)
IF [%4] == [] (
	SET COPY_DLLS=false
) ELSE (
	SET COPY_DLLS=%4
)
IF [%5] == [] (
	SET CONFIG=Release
) ELSE (
	SET CONFIG=%5
)
SET LIBCFG=Release
IF "%CONFIG%" == "Debug" (
	SET LIBCFG=Debug
)

ECHO PostBuild: copying resource files - Source Directory: %SOURCE_DIR% ; Output Directory: %OUT_DIR% ; Platform: %PLATFORM% ; copy_dlls? %COPY_DLLS% ; Configuration: %CONFIG%

IF "%COPY_DLLS%" == "true" (
	copy "%SOURCE_DIR%\..\dependancies\%PLATFORM%\gtkmm\%LIBCFG%\*.dll" "%OUT_DIR%\" /y /D
	copy "%SOURCE_DIR%\..\dependancies\%PLATFORM%\rttr\%LIBCFG%\*.dll" "%OUT_DIR%\" /y /D
	
	IF "%CONFIG%" == "Debug" (
		copy "%SOURCE_DIR%\..\dependancies\%PLATFORM%\gtkmm\%LIBCFG%\*.pdb" "%OUT_DIR%\" /y /D
		copy "%SOURCE_DIR%\..\dependancies\%PLATFORM%\rttr\%LIBCFG%\*.pdb" "%OUT_DIR%\" /y /D
	)
)

rem copy config files
xcopy "%SOURCE_DIR%\..\resources\config" "%OUT_DIR%\..\config" /s/i/y/d

rem copy gtk resources
xcopy "%SOURCE_DIR%\..\resources\packaged" "%OUT_DIR%\.." /s/i/y/d

ECHO --
ECHO --

exit