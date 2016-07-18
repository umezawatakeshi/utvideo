/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#pragma once

#include "print_log_value.h"

/*
 * デコードテスト
 * 
 * １つのテストに対して以下のものが必要
 *
 * 入力クリップ（入力フォーマット＝コーデックのFourCCも決まる）
 * 出力クリップ（出力フォーマットも決まる）
 * 許容誤差
 *
 * 入力クリップに対して複数の 出力クリップ,許容誤差 のペアがありうる
 */

extern vector<tuple<string, string, unsigned int>> vecDecodeClips;
