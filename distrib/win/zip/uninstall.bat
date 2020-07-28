@echo off
cd /d %~dp0

if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (

start /wait %windir%\System32\regsvr32.exe /s /u %windir%\System32\utv_dmo.dll
del %windir%\System32\utv_core.dll
del %windir%\System32\utv_vcm.dll
del %windir%\System32\utv_dmo.dll

start /wait %windir%\SysWOW64\regsvr32.exe /s /u %windir%\System32\utv_dmo.dll
del %windir%\SysWOW64\utv_core.dll
del %windir%\SysWOW64\utv_vcm.dll
del %windir%\SysWOW64\utv_dmo.dll

start /wait %windir%\regedit.exe /s utvideo-x64-unreg.reg

) else (

@rem note: x86-windows is out of support
echo This processor architecture is not supported.
call :notsup
exit /b 1

)

exit /b 0

:notsup
echo WScript.Quit(MsgBox("This processor architecture is not supported.",vbOK,"Ut Video Codec Suite batchfile installer")) > %TEMP%\utvideo-arch-notsup.vbs & start /wait %TEMP%\utvideo-arch-notsup.vbs
del %TEMP%\utvideo-arch-notsup.vbs
exit /b
