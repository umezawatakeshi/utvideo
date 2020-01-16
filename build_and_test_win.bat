setlocal
del tests-*.xml
set MSBUILD="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"

%MSBUILD% utvideo.sln /p:Configuration=Release /p:Platform=Win32 /t:Rebuild
if ERRORLEVEL 1 goto errexit

%MSBUILD% utvideo.sln /p:Configuration=Release /p:Platform=x64 /t:Rebuild
if ERRORLEVEL 1 goto errexit

"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" setup_win\setup_win.iss
if ERRORLEVEL 1 goto errexit

set ERRLVL=0

Release\tests.exe --log_format=XML --log_sink=tests-x86.xml --log_level=all
if ERRORLEVEL 1 set ERRLVL=%ERRORLEVEL%

x64\Release\tests.exe --log_format=XML --log_sink=tests-x64.xml --log_level=all
if ERRORLEVEL 1 set ERRLVL=%ERRORLEVEL%

if "%ERRLVL%" NEQ "0" goto errexit

exit /b 0

:errexit
exit /b 1
