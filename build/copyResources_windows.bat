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

ECHO PostBuild: copying resource files - Source Directory: %SOURCE_DIR% ; Output Directory: %OUT_DIR% ; Platform: %PLATFORM% ; copy_dlls? %COPY_DLLS%

IF "%COPY_DLLS%" == "true" (
	copy "%SOURCE_DIR%\..\dependancies\%PLATFORM%\gtkmm\*.dll" "%OUT_DIR%\" /y /D
)

REM xcopy "%SOURCE_DIR%\Synthesizer\Resources" "%OUT_DIR%\Resources" /s/i/y/d
xcopy "%SOURCE_DIR%\Synthesizer\Config" "%OUT_DIR%\..\config" /s/i/y/d

rem copy gtk resources
xcopy "%SOURCE_DIR%\Synthesizer\Resources\share" "%OUT_DIR%\..\share" /s/i/y/d
xcopy "%SOURCE_DIR%\Synthesizer\Resources\lib" "%OUT_DIR%\..\lib" /s/i/y/d

ECHO --
ECHO --

exit