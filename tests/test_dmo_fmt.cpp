/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"

#pragma warning(disable:4073)
#pragma init_seg(lib)

#include "test_dmo_fmt.h"

vector<wstring> vecEncoderName = {
	L"UtVideo RGB DMO",
	L"UtVideo RGBA DMO",
	L"UtVideo YUV422 BT.601 DMO",
	L"UtVideo YUV420 BT.601 DMO",
	L"UtVideo YUV422 BT.709 DMO",
	L"UtVideo YUV420 BT.709 DMO",
	L"UtVideo Pro YUV422 10bit DMO",
};

vector<wstring> vecDecoderName = {
	L"ULRG Decoder DMO",
	L"ULRA Decoder DMO",
	L"ULY2 Decoder DMO",
	L"ULY0 Decoder DMO",
	L"ULH2 Decoder DMO",
	L"ULH0 Decoder DMO",
	L"UQY2 Decoder DMO",
};

#define DEFINE_FOURCC_GUID(fcc, val) FourCCGUID MEDIASUBTYPE_ ## fcc(val)

DEFINE_FOURCC_GUID(YUNV, FCC('YUNV'));
DEFINE_FOURCC_GUID(yuvs, FCC('yuvs'));
DEFINE_FOURCC_GUID(HDYC, FCC('HDYC'));
DEFINE_FOURCC_GUID(UYNV, FCC('UYNV'));
DEFINE_FOURCC_GUID(2vuy, FCC('2vuy'));
DEFINE_FOURCC_GUID(v210, FCC('v210'));

vector<vector<GUID>> vecSupportedEncoderInputTypes = {
	{ MEDIASUBTYPE_RGB24, MEDIASUBTYPE_RGB32, },
	{ MEDIASUBTYPE_ARGB32, MEDIASUBTYPE_RGB32, },
	{ MEDIASUBTYPE_YUY2, MEDIASUBTYPE_YUYV, MEDIASUBTYPE_YUNV, MEDIASUBTYPE_yuvs, MEDIASUBTYPE_UYVY, MEDIASUBTYPE_UYNV, MEDIASUBTYPE_2vuy, MEDIASUBTYPE_RGB24, MEDIASUBTYPE_RGB32, },
	{ MEDIASUBTYPE_YV12, MEDIASUBTYPE_YUY2, MEDIASUBTYPE_YUYV, MEDIASUBTYPE_YUNV, MEDIASUBTYPE_yuvs, MEDIASUBTYPE_UYVY, MEDIASUBTYPE_UYNV, MEDIASUBTYPE_2vuy, MEDIASUBTYPE_RGB24, MEDIASUBTYPE_RGB32, },
	{ MEDIASUBTYPE_HDYC, MEDIASUBTYPE_YUY2, MEDIASUBTYPE_YUYV, MEDIASUBTYPE_YUNV, MEDIASUBTYPE_yuvs, MEDIASUBTYPE_UYVY, MEDIASUBTYPE_UYNV, MEDIASUBTYPE_2vuy, MEDIASUBTYPE_RGB24, MEDIASUBTYPE_RGB32, },
	{ MEDIASUBTYPE_YV12, MEDIASUBTYPE_HDYC, MEDIASUBTYPE_YUY2, MEDIASUBTYPE_YUYV, MEDIASUBTYPE_YUNV, MEDIASUBTYPE_yuvs, MEDIASUBTYPE_UYVY, MEDIASUBTYPE_UYNV, MEDIASUBTYPE_2vuy, MEDIASUBTYPE_RGB24, MEDIASUBTYPE_RGB32, },
	{ MEDIASUBTYPE_v210, },
};

vector<vector<GUID>> vecSupportedDecoderOutputTypes = vecSupportedEncoderInputTypes;

vector<IID> vecSupportedEncoderInterfaces = { IID_IMediaObject, IID_IPersistStream, IID_IAMVfwCompressDialogs };
vector<IID> vecSupportedDecoderInterfaces = { IID_IMediaObject };

DWORD DirectShowFormatToVCMFormat(REFGUID subtype)
{
	if (IsEqualGUID(subtype, MEDIASUBTYPE_RGB24))
		return 24;
	else if (IsEqualGUID(subtype, MEDIASUBTYPE_RGB32))
		return 32;
	else if (IsEqualGUID(subtype, MEDIASUBTYPE_ARGB32))
		return 32;

	GUID tmp = subtype;
	tmp.Data1 = FCC('YUY2');
	if (IsEqualGUID(tmp, MEDIASUBTYPE_YUY2))
		return subtype.Data1;

	BOOST_CHECK(false);
	return -1;
}

HRESULT hrCoInit = CoInitializeEx(NULL, COINIT_MULTITHREADED);
