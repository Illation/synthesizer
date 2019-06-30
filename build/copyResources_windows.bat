@ECHO OFF

SET SOURCE_DIR=%1
SET OUT_DIR=%2

SET USER_DIR_RELATIVE=..\..\..\user_data\
SET USER_DIR_JSON=%USER_DIR_RELATIVE:\=/%
SET USER_DIR=%OUT_DIR%\%USER_DIR_RELATIVE%

if not exist "%USER_DIR%" mkdir %USER_DIR%
ECHO user directory: %USER_DIR%

ECHO PostBuild: copying resource files - Source Directory: %SOURCE_DIR% ; Output Directory: %OUT_DIR% 

rem copy config files
xcopy "%SOURCE_DIR%\..\resources\config" "%USER_DIR%" /s/i/y/d

rem copy gtk resources
xcopy "%SOURCE_DIR%\..\resources\packaged" "%OUT_DIR%\.." /s/i/y/d

ECHO --
ECHO --

exit