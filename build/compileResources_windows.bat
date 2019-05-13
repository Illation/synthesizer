@ECHO OFF

SET SOURCE_DIR=%1

SET GEN_DIR=%SOURCE_DIR%/Synthesizer/UI/_generated/
SET RES_DIR=%SOURCE_DIR%/Synthesizer/Config/UI/
SET TOOL_DIR=%SOURCE_DIR%\..\dependancies\tools\

ECHO Prebuild: Generating compiled glib sources - Source Directory: %RES_DIR%  ; Generated Directory: %GEN_DIR%

if not exist "%GEN_DIR%" mkdir %GEN_DIR%

setlocal 
  set XMLLINT=%TOOL_DIR%xmllint\

echo %TOOL_DIR%glib-compile-resources.exe --target=%GEN_DIR%resources.c --sourcedir=%RES_DIR% --generate-source %RES_DIR%synthesizer.gresource.xml
%TOOL_DIR%glib-compile-resources.exe --target=%GEN_DIR%resources.c --sourcedir=%RES_DIR% --generate-source %RES_DIR%synthesizer.gresource.xml

endlocal

ECHO --
ECHO --
