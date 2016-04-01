/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once


namespace boost { namespace test_tools { namespace tt_detail {

template<>
struct print_log_value<GUID>
{
	void operator()(ostream& os, const GUID& guid)
	{
		RPC_CSTR psz;

		UuidToString(&guid, &psz);
		os << (char *)psz;
		RpcStringFree(&psz);
	}
};

template<typename T>
struct print_log_value<vector<T>>
{
	void operator()(ostream& os, const vector<T>& vec)
	{
		print_log_value<T> ple;

		if (vec.empty())
		{
			os << "{}";
		}
		else
		{
			os << "{ ";
			ple(os, vec.front());
			for (auto it = vec.begin() + 1, end = vec.end(); it != end; ++it)
			{
				os << ", ";
				ple(os, *it);
			}
			os << " }";
		}
	}
};

}}}

extern vector<wstring> vecEncoderName;
extern vector<wstring> vecDecoderName;
extern vector<vector<GUID>> vecSupportedEncoderInputTypes;
extern vector<vector<GUID>> vecSupportedDecoderOutputTypes;
extern vector<vector<GUID>> vecUnsupportedEncoderInputTypes;
extern vector<vector<GUID>> vecUnsupportedDecoderOutputTypes;
extern vector<GUID> vecTopPriorityEncoderInputType;
extern vector<IID> vecSupportedEncoderInterfaces;
extern vector<IID> vecSupportedDecoderInterfaces;

class DMOEncoderCLSID : public GUID
{
public:
	DMOEncoderCLSID(DWORD fcc) : GUID({ fcc, 0xE991, 0x460D, 0x84, 0x0B, 0xC1, 0xC6, 0x49, 0x74, 0x57, 0xEF }) {}
};

class DMODecoderCLSID : public GUID
{
public:
	DMODecoderCLSID(DWORD fcc) : GUID({ fcc, 0xD991, 0x460D, 0x84, 0x0B, 0xC1, 0xC6, 0x49, 0x74, 0x57, 0xEF }) {}
};

class FourCCGUID : public GUID
{
public:
	FourCCGUID(DWORD fcc) : GUID({ fcc, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }) {}
};

DWORD DirectShowFormatToVCMFormat(REFGUID subtype);
