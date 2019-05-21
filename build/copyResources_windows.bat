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

SET USER_DIR_RELATIVE=..\..\..\user_data\
SET USER_DIR_JSON=%USER_DIR_RELATIVE:\=/%
SET USER_DIR=%OUT_DIR%\%USER_DIR_RELATIVE%

if not exist "%USER_DIR%" mkdir %USER_DIR%
ECHO user directory: %USER_DIR%

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
xcopy "%SOURCE_DIR%\..\resources\config" "%USER_DIR%" /s/i/y/d

rem create the pointer to the user data directory
if not exist "%OUT_DIR%\..\config\" mkdir %OUT_DIR%\..\config\
@echo off
  echo {"dir pointer":{"user dir path":"%USER_DIR_JSON%"}}>"%OUT_DIR%\..\config\userDirPointer.json"

rem copy gtk resources
xcopy "%SOURCE_DIR%\..\resources\packaged" "%OUT_DIR%\.." /s/i/y/d

ECHO --
ECHO --

exit