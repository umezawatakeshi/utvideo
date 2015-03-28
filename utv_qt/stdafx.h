/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

/*
* QuickTime SDK for Windows に含まれる、
* 古い Visual C++ のために用意されている
* <GNUCompatibility/stdint.h> と
* <GNUCompatibility/stdbool.h> の
* include を阻止する。
*/
#ifdef _MSC_VER
#define _STDINT_H 1
#define __STDBOOL_H__ 1
#endif

/*
* Windows 8/Server2012 で GetProcessInformation が追加されていて、
* それが衝突するので Mac の方をリネームする。
* どちらも呼ばないのでコンパイルエラーにならなければそれでいい。
*/
#ifdef _WIN32
#define GetProcessInformation MacGetProcessInformation
#endif

/*
* QuickTime SDK for Windows と
* QuickTime SDK for Mac とでディレクトリが違う。
* こういうのやめてほしい。
*/
#ifdef _WIN32
#include <QuickTimeComponents.h>
#include <ImageCodec.h>
#else
#include <QuickTime/QuickTimeComponents.h>
#include <QuickTime/ImageCodec.h>
#endif
