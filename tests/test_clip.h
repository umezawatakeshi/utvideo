/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#pragma once

#include "print_log_value.h"

/*
 * デコードテスト
 * 
 * 入力クリップ（入力フォーマット＝コーデックのFourCCも決まる）
 * 出力クリップ（出力フォーマットも決まる）
 * 許容誤差
 */
extern const vector<tuple<string, string, unsigned int>> vecDecodeClips;

/*
 * エンコードテスト
 *
 * 入力クリップ（入力フォーマットも決まる）
 * 出力クリップ（出力フォーマット＝コーデックの FourCC や extradata も決まる）
 * エンコーダのコンフィギュレーションデータ
 * 出力クリップのストリームヘッダの extradata の比較マスク
 */
extern const vector<tuple<string, string, vector<uint8_t>, vector<uint8_t>>> vecEncodeClips;

/*
 * エンコードデコードテスト
 * 入力クリップ（入力フォーマットも決まる）
 * 出力クリップ（出力フォーマットも決まる）
 * コーデックの FourCC
 * エンコーダのコンフィギュレーションデータ
 * 許容誤差
 */
extern const vector<tuple<string, string, uint32_t, vector<uint8_t>, unsigned int>> vecEncDecClips;
