/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

#include "print_log_value.h"

extern const vector<wstring> vecEncoderName;
extern const vector<wstring> vecDecoderName;
extern const vector<vector<GUID>> vecSupportedEncoderInputSubtypes;
extern const vector<vector<GUID>> vecSupportedDecoderOutputSubtypes;
extern const vector<vector<GUID>> vecUnsupportedEncoderInputSubtypes;
extern const vector<vector<GUID>> vecUnsupportedDecoderOutputSubtypes;
extern const vector<GUID> vecTopPriorityEncoderInputSubtype;
extern const vector<IID> vecSupportedEncoderInterfaces;
extern const vector<IID> vecSupportedDecoderInterfaces;

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
