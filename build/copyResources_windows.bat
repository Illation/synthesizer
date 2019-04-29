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

ECHO Source Directory: %SOURCE_DIR% ; Output Directory: %OUT_DIR% ; Platform: %PLATFORM% ; copy_dlls? %COPY_DLLS%

IF "%COPY_DLLS%" == "true" (
	copy "%SOURCE_DIR%\..\dependancies\%PLATFORM%\gtk\*.dll" "%OUT_DIR%\" /y /D
	IF "%PLATFORM%" == "x32" (
		copy "%SOURCE_DIR%\..\dependancies\%PLATFORM%\portaudio\portaudio_x86.dll" "%OUT_DIR%\portaudio_x86.dll" /y /D
	) ELSE (
		copy "%SOURCE_DIR%\..\dependancies\%PLATFORM%\portaudio\portaudio_x64.dll" "%OUT_DIR%\portaudio_x64.dll" /y /D
	)
)

REM xcopy "%SOURCE_DIR%\Synthesizer\Resources" "%OUT_DIR%\Resources" /s/i/y/d
xcopy "%SOURCE_DIR%\Synthesizer\Config" "%OUT_DIR%" /s/i/y/d

rem copy gtk resources
xcopy "%SOURCE_DIR%\Synthesizer\Resources\gtk-3.20" "%OUT_DIR%\share\themes\Windows10\gtk-3.0" /s/i/y/d
xcopy "%SOURCE_DIR%\Synthesizer\Resources\Adwaita" "%OUT_DIR%\share\icons\Adwaita" /s/i/y/d
rem copy "%SOURCE_DIR%\Synthesizer\Resources\gschema.dtd" "%OUT_DIR%\share\glib-2.0\schemas\" /y /D

exit