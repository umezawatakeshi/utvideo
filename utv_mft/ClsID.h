/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */


#define DEFINE_MFTCODEC_CLSID(fcc, fccname) \
	DEFINE_GUID(CLSID_##fccname##MFTEncoder, fcc, 0xE992, 0x460D, 0x84, 0x0B, 0xC1, 0xC6, 0x49, 0x74, 0x57, 0xEF); \
	DEFINE_GUID(CLSID_##fccname##MFTDecoder, fcc, 0xD992, 0x460D, 0x84, 0x0B, 0xC1, 0xC6, 0x49, 0x74, 0x57, 0xEF)

DEFINE_MFTCODEC_CLSID(FCC('ULRA'), ULRA);
DEFINE_MFTCODEC_CLSID(FCC('ULRG'), ULRG);
DEFINE_MFTCODEC_CLSID(FCC('ULY0'), ULY0);
DEFINE_MFTCODEC_CLSID(FCC('ULY2'), ULY2);
