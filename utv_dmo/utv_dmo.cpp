/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

// utv_dmo.cpp : DLL エクスポートの実装です。


#include "stdafx.h"
#include "resource.h"
#include "utv_dmo.h"


class CUtVideoDMOModule : public CAtlDllModuleT<CUtVideoDMOModule>
{
public :
	DECLARE_LIBID(LIBID_UtVideoDMO)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_UTVIDEODMO, "{E625D10C-D936-4469-9A2D-93F921C29DF9}")
};

CUtVideoDMOModule _AtlModule;


#ifdef _MANAGED
#pragma managed(push, off)
#endif

// DLL エントリ ポイント
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		LOGPRINTF("DllMain(hInstance=%" PRIp ", dwReason=DLL_PROCESS_ATTACH lpReserved=%" PRIp, hInstance, lpReserved);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		LOGPRINTF("DllMain(hInstance=%" PRIp ", dwReason=DLL_PROCESS_DETACH lpReserved=%" PRIp, hInstance, lpReserved);
	}

	hInstance;
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

