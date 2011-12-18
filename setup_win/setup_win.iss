; �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e
; $Id$

[Setup]
AppName=Ut Video Codec Suite
AppId=utvideo
AppVersion=10.2.1
AppCopyright=Copyright (c) 2008-2011 UMEZAWA Takeshi
AppPublisher=UMEZAWA Takeshi
AppPublisherURL=http://umezawa.dyndns.info/
UninstallDisplayName=Ut Video Codec Suite
DefaultDirName={pf}\utvideo
DefaultGroupName=Ut Video Codec Suite
SolidCompression=yes
OutputBaseFilename=utvideo-win
ArchitecturesInstallIn64BitMode=x64
LicenseFile=..\gplv2.rtf
InfoAfterFile=info.en.rtf

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl";
Name: "ja"; MessagesFile: "compiler:Languages\Japanese.isl"; InfoAfterFile: "info.ja.rtf"

[Files]
Source: "..\Release\utv_core.dll";     DestDir: "{sys}"; Flags: ignoreversion 32bit;
Source: "..\Release\utv_vcm.dll";      DestDir: "{sys}"; Flags: ignoreversion 32bit;
Source: "..\Release\utv_dmo.dll";      DestDir: "{sys}"; Flags: ignoreversion 32bit regserver;
Source: "..\x64\Release\utv_core.dll"; DestDir: "{sys}"; Flags: ignoreversion 64bit;           Check: Is64BitInstallMode
Source: "..\x64\Release\utv_vcm.dll";  DestDir: "{sys}"; Flags: ignoreversion 64bit;           Check: Is64BitInstallMode
Source: "..\x64\Release\utv_dmo.dll";  DestDir: "{sys}"; Flags: ignoreversion 64bit regserver; Check: Is64BitInstallMode
Source: "..\Release\utv_cfg.exe";      DestDir: "{app}"; Flags: ignoreversion;
Source: "..\gplv2.rtf";                DestDir: "{app}"; Flags: ignoreversion;
Source: "..\gplv2.txt";                DestDir: "{app}"; Flags: ignoreversion;
Source: "..\gplv2.ja.sjis.txt";        DestDir: "{app}"; Flags: ignoreversion;
Source: "..\readme.en.html";           DestDir: "{app}"; Flags: ignoreversion;
Source: "..\readme.ja.html";           DestDir: "{app}"; Flags: ignoreversion;

[Registry]
Root: HKLM; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32";             ValueName: "VIDC.ULRA"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32";             ValueName: "VIDC.ULRG"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32";             ValueName: "VIDC.ULY0"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32";             ValueName: "VIDC.ULY2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue
Root: HKLM; Subkey: "Software\Wow6432Node\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULRA"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM; Subkey: "Software\Wow6432Node\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULRG"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM; Subkey: "Software\Wow6432Node\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULY0"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode
Root: HKLM; Subkey: "Software\Wow6432Node\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueName: "VIDC.ULY2"; ValueType: string; ValueData: "{sys}\utv_vcm.dll"; Flags: uninsdeletevalue; Check: Is64BitInstallMode

[Icons]
Name: "{group}\Global Configuration";             Filename: "{app}\utv_cfg.exe"
Name: "{group}\License Agreement (English, RTF)"; Filename: "{app}\gplv2.rtf"
Name: "{group}\License Agreement (English)";      Filename: "{app}\gplv2.txt"
Name: "{group}\License Agreement (Japanese)";     Filename: "{app}\gplv2.ja.sjis.txt"
Name: "{group}\Readme (English)";                 Filename: "{app}\readme.en.html"
Name: "{group}\Readme (Japanese)";                Filename: "{app}\readme.ja.html"

[Run]
Filename: "{app}\readme.en.html"; Description: "View readme";   Flags: postinstall shellexec skipifsilent; Languages: en
Filename: "{app}\readme.ja.html"; Description: "readme �̕\��"; Flags: postinstall shellexec skipifsilent; Languages: ja
