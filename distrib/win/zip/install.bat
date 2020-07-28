@echo off
cd /d %~dp0

echo If you agree to license terms written in readme.en.html (or readme.ja.html) and gplv2.txt, press any key three times to continue installation.
echo If you don't agree to it, close this window immediately to cancel installation.
echo;
pause
pause
pause

if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (

copy x64\utv_core.dll %windir%\System32\
copy x64\utv_vcm.dll %windir%\System32\
copy x64\utv_dmo.dll %windir%\System32\
start /wait %windir%\System32\regsvr32.exe /s %windir%\System32\utv_dmo.dll

copy x86\utv_core.dll %windir%\SysWOW64\
copy x86\utv_vcm.dll %windir%\SysWOW64\
copy x86\utv_dmo.dll %windir%\SysWOW64\
start /wait %windir%\SysWOW64\regsvr32.exe /s %windir%\System32\utv_dmo.dll

start /wait %windir%\regedit.exe /s utvideo-x64.reg

) else (

@rem note: x86-windows is out of support
echo This processor architecture is not supported.
call :notsup
exit /b 1

)

start "" readme.en.html
start "" readme.ja.html
start "" http://umezawa.dyndns.info/wordpress/?page_id=1577
start "" http://umezawa.dyndns.info/wordpress/?page_id=1565
start "" http://seiga.nicovideo.jp/seiga/im1922939

exit /b 0

:notsup
echo WScript.Quit(MsgBox("This processor architecture is not supported.",vbOK,"Ut Video Codec Suite batchfile installer")) > %TEMP%\utvideo-arch-notsup.vbs & start /wait %TEMP%\utvideo-arch-notsup.vbs
del %TEMP%\utvideo-arch-notsup.vbs
exit /b
