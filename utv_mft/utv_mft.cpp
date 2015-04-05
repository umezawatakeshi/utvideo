/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// utv_mft.cpp : DLL エクスポートの実装です。


#include "stdafx.h"
#include "resource.h"
#include "utv_mft.h"


class CUtVideoMFTModule : public CAtlDllModuleT<CUtVideoMFTModule>
{
public :
	DECLARE_LIBID(LIBID_UtVideoMFT)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_UTVIDEOMFT, "{274346FB-B683-4FE3-905A-0A63165E1F98}")
};

CUtVideoMFTModule _AtlModule;


#ifdef _MANAGED
#pragma managed(push, off)
#endif

// DLL エントリ ポイント
extern "C" BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		LOGPRINTF("DllMain(hModule=%" PRIp ", dwReason=DLL_PROCESS_ATTACH lpReserved=%" PRIp, hModule, lpReserved);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		LOGPRINTF("DllMain(hModule=%" PRIp ", dwReason=DLL_PROCESS_DETACH lpReserved=%" PRIp, hModule, lpReserved);
	}

	return _AtlModule.DllMain(dwReason, lpReserved);
}

#ifdef _MANAGED
#pragma managed(pop)
#endif




// DLL を OLE によってアンロードできるようにするかどうかを指定します。
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// 要求された型のオブジェクトを作成するクラス ファクトリを返します。
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - エントリをシステム レジストリに追加します。
STDAPI DllRegisterServer(void)
{
    // オブジェクト、タイプ ライブラリおよびタイプ ライブラリ内のすべてのインターフェイスを登録します
    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - エントリをレジストリから削除します。
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

