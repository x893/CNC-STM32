@ECHO OFF
SET KEIL=C:\Tools\Keil5\UV4
SET OUTDIR=%CD%\out

IF "%1*"=="build*"   GOTO :BUILD
IF "%1*"=="clean*"   GOTO :CLEAN
IF "%1*"=="rebuild*" GOTO :REBUILD
GOTO :BUILD

:CLEAN
FOR %%i IN ("%OUTDIR%\*.*") DO IF NOT "%%~nxi"=="cnc.log" DEL /F /S /Q "%%i" >nul
GOTO :DONE

:REBUILD
CALL :CLEAN

:BUILD
SET ERRFILE="%OUTDIR%\errors.txt"
"%SystemRoot%\System32\cmd.exe" /C "%KEIL%\Uv4.exe -j0 -b cnc.uvproj -t cnc -o %ERRFILE%
TYPE %ERRFILE% | "%SystemRoot%\System32\find.exe" /I "error:"
DEL /Q %ERRFILE%

:DONE
IF EXIST "%~dp2obj" rmdir /Q /S "%~dp2obj"
SET ERRORLEVEL=0
EXIT /B 0
