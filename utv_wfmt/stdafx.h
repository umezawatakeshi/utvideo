// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#ifndef _WIN32_WINNT		// Windows XP 以降のバージョンに固有の機能の使用を許可します。                   
#define _WIN32_WINNT 0x0501	// これを Windows の他のバージョン向けに適切な値に変更してください。
#endif						

#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから使用されていない部分を除外します。



// TODO: プログラムに必要な追加ヘッダーをここで参照してください。
#include <windows.h>
#include <uuids.h> // for MEDIATYPE_* and MEDIASUBTYPE_*

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <crtdbg.h> // XXX
