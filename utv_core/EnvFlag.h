/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#pragma once

/**
 * 環境変数が、
 *   なければ false
 *   空文字列なら false
 *   "1" なら true
 *   それ以外の場合は未定義
 */
bool GetEnvFlagBool(const char* pszEnvName);
