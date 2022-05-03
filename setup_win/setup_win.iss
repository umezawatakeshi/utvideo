; �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e
; $Id$

#include "../utv_core/version.h"

[Setup]
AppName=Ut Video Codec Suite
AppId=utvideo
AppVersion="{#UTVIDEO_VERSION_MAJOR}.{#UTVIDEO_VERSION_MINOR}.{#UTVIDEO_VERSION_BUGFIX}"
AppCopyright=Copyright (c) 2008-2022 UMEZAWA Takeshi
AppPublisher=UMEZAWA Takeshi
AppPublisherURL=http://umezawa.dyndns.info/
UninstallDisplayName=Ut Video Codec Suite
DefaultDirName={pf}\utvideo
DefaultGroupName=Ut Video Codec Suite
SolidCompression=yes
OutputBaseFilename="utvideo-{#UTVIDEO_VERSION_MAJOR}.{#UTVIDEO_VERSION_MINOR}.{#UTVIDEO_VERSION_BUGFIX}-win"
ArchitecturesInstallIn64BitMode=x64
LicenseFile=..\gplv2.rtf
InfoBeforeFile=before.en.rtf
InfoAfterFile=info.en.rtf
DisableWelcomePage=no

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl";
Name: "ja"; MessagesFile: "compiler:Languages\Japanese.isl"; InfoBeforeFile: "before.ja.rtf"; InfoAfterFile: "info.ja.rtf"

[Files]
Source: "..\Release\utv_core.dll";     DestDir: "{sys}"; Flags: ignoreversion 32bit;
Source: "..\Release\utv_vcm.dll";      DestDir: "{sys}"; Flags: ignoreversion 32bit;
Source: "..\Release\utv_dmo.dll";      DestDir: "{sys}"; Flags: ignoreversion 32bit regserver;
Source: "..\x64\Release\utv_core.dll"; DestDir: "{sys}"; Flags: ignoreversion 64bit;           Check: Is64BitInstallMode
Source: "..\x64\Release\utv_vcm.dll";  DestDir: "{sys}"; Flags: ignoreversion 64bit;           Check: Is64BitInstallMode
Source: "..\x64\Release\utv_dmo.dll";  DestDir: "{sys}"; Flags: ignoreversion 64bit regserver; Check: Is64BitInstallMode
Source: "..\gplv2.rtf";                DestDir: "{app}"; Flags: ignoreversion;
Source: "..\gplv2.txt";                DestDir: "{app}"; Flags: ignoreversion;
Source: "..\gplv2.ja.sjis.txt";        DestDir: "{app}"; Flags: ignoreversion;
Source: "..\style.css";                DestDir: "{app}"; Flags: ignoreversion;
Source: "..\readme.en.html";           DestDir: "{app}"; Flags: ignoreversion;
Source: "..\readme.ja.html";           DestDir: "{app}"; Flags: ignoreversion;

[InstallDelete]
Name: "{app}\utv_cfg.exe"; Type: files
Name: "{app}\utvideo.reg"; Type: files

[Registry]
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULRA"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULRG"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULY0"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULY2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULY4"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULH0"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULH2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULH4"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UQY0"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UQY2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UQRG"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UQRA"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UMRA"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UMRG"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UMY2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UMY4"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UMH2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UMH4"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULRA"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULRG"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULY0"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULY2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULY4"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULH0"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULH2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULH4"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UQY0"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UQY2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UQRG"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UQRA"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UMRA"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UMRG"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UMY2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UMY4"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UMH2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.UMH4"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode

[Icons]
Name: "{group}\Global Configuration";              Filename: "{sys}\rundll32.exe"; Parameters: "{sys}\utv_core.dll,GlobalConfigDialog"
Name: "{group}\License Agreement (English, RTF)";  Filename: "{app}\gplv2.rtf"
Name: "{group}\License Agreement (English)";       Filename: "{app}\gplv2.txt"
Name: "{group}\License Agreement (Japanese)";      Filename: "{app}\gplv2.ja.sjis.txt"
Name: "{group}\Readme (English)";                  Filename: "{app}\readme.en.html"
Name: "{group}\Readme (Japanese)";                 Filename: "{app}\readme.ja.html"

[Run]
Filename: "{app}\readme.en.html"; Description: "View readme";   Flags: postinstall shellexec skipifsilent; Languages: en
Filename: "{app}\readme.ja.html"; Description: "readme �̕\��"; Flags: postinstall shellexec skipifsilent; Languages: ja

Filename: "http://umezawa.dyndns.info/wordpress/?page_id=1577"; Description: "View ""About Donations"" page"; Flags: postinstall shellexec skipifsilent; Languages: en
Filename: "http://umezawa.dyndns.info/wordpress/?page_id=1565"; Description: "�u��t�ɂ��āv�y�[�W�̕\��";  Flags: postinstall shellexec skipifsilent; Languages: ja

Filename: "http://seiga.nicovideo.jp/seiga/im1922939"; Description: "niconico �̃R���e���c�c���[�e��i�w��p�̍�i�y�[�W�̕\��"; Flags: postinstall shellexec skipifsilent; Languages: ja

; �ȑO�̃o�[�W�����ŃC���X�g�[������ MFT �ł� unregister ����BDLL �̍폜�́i�߂�ǂ������̂Łj������߂��B
Filename: "{sys}\regsvr32.exe"; Parameters: "/u /s {sys}\utv_mft.dll"; Flags: runascurrentuser 32bit;                            MinVersion: 0, 6.0
Filename: "{sys}\regsvr32.exe"; Parameters: "/u /s {sys}\utv_mft.dll"; Flags: runascurrentuser 64bit; Check: Is64BitInstallMode; MinVersion: 0, 6.0
