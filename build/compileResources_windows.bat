@ECHO OFF

SET SOURCE_DIR=%1

SET GEN_DIR=%SOURCE_DIR%/Synthesizer/UI/_generated/
SET RES_DIR=%SOURCE_DIR%/Synthesizer/Config/UI/
SET TOOL_DIR=%SOURCE_DIR%\..\dependancies\tools\

SET TARGET_FILE=%GEN_DIR%resources.c 

ECHO Prebuild: Generating compiled glib sources - Source Directory: %RES_DIR%  ; Generated Directory: %GEN_DIR%

rem make sure generated Directory exists

if not exist "%GEN_DIR%" mkdir "%GEN_DIR%"

rem set the xmllinter tool
setlocal 
  set XMLLINT=%TOOL_DIR%xmllint\xmllint.exe

rem compile our resources
%TOOL_DIR%glib-compile-resources.exe --target=%TARGET_FILE% --sourcedir=%RES_DIR% --generate-source %RES_DIR%synthesizer.gresource.xml
%TOOL_DIR%glib-compile-resources.exe --target=%GEN_DIR%resources.h --sourcedir=%RES_DIR% --generate-header %RES_DIR%synthesizer.gresource.xml

endlocal

ECHO --
ECHO --
