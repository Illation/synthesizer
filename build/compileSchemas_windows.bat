@ECHO OFF

SET SOURCE_DIR=%1
SET OUT_DIR=%2

SET GEN_DIR=%OUT_DIR%\..\share\glib-2.0\schemas\
SET RES_DIR=%SOURCE_DIR%\Synthesizer\Config\UI\schemas\

ECHO --
ECHO --

ECHO PostBuild: compiling schema files - Source Directory: %RES_DIR% ; Output Directory: %GEN_DIR% ; 

if not exist "%GEN_DIR%" mkdir %GEN_DIR%

%SOURCE_DIR%\..\dependancies\tools\glib-compile-schemas.exe --targetdir=%GEN_DIR% %RES_DIR%

ECHO --
ECHO --