/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#include "stdafx.h"

/*
 * Intel SDE (Software Development Emulator) for Windows 7.49.0 上で動かすと、
 * LoadLibrary/FreeLibrary を繰り返すとそれだけでメモリリークするという問題がある。
 * このテストではコーデックの DLL をロード/アンロードを繰り返すため、この問題の影響を大きく受ける。
 * 32bit だとユーザーのアドレス空間が 2GB しかないので、そのうちアドレス空間を埋め尽くしてクラッシュする。
 *
 * あらかじめロードしておいておけば毎回アンロードされないのでこの問題の影響を受けなくなる。
 * …はずであるが、だいぶマシになったもののこれでもまだメモリ使用量が増えていく現象が確認されている。
 *
 * ちなみに、ロードするたびにバイナリ変換が走るらしく、
 * アンロードされないようにしておくと実行が速くなる。
 *
 * アンロードした時にバイナリ変換した結果が捨てられるはずが
 * ちゃんと捨てられずにリークしている…のではないかと思われる。
 *
 * Windows 用以外の SDE でどうなるかは調べていない。
 */

class CKeepLibrary
{
private:
	std::vector<HMODULE> vecLibs;

public:
	CKeepLibrary(std::vector<const char *> vecLibNames)
	{
		vecLibs.reserve(vecLibNames.size());
		for (auto name : vecLibNames)
		{
			HMODULE hModule = LoadLibrary(name);
			if (hModule != NULL)
			{
				vecLibs.push_back(hModule);
			}
			else
			{
				DWORD err = GetLastError();
				char* p;
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&p, 0, NULL);
				fprintf(stderr, "CKeepLibrary: LoadLibrary(%s) failed: GetLastError()=%d %s", name, GetLastError(), p);
				LocalFree(p);
			}
		}
	}

	~CKeepLibrary()
	{
		for (auto hModule : vecLibs)
		{
			FreeLibrary(hModule);
		}
	}
} kl({
	"C:\\windows\\system32\\utv_vcm.dll", // VCM コーデックは常に system32 の下にあるものが使われる
	"C:\\windows\\system32\\utv_dmo.dll", // DMO コーデックは system32 にあるものと tests.exe の横にあるものとかどちらが使われるかは場合によるので、両方ロードしておく
	"utv_dmo.dll",
});
