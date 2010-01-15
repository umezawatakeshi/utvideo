/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

struct HUFFMAN_ENCODE_TABLE;
struct HUFFMAN_DECODE_TABLE;

struct TUNEDFUNC
{
	void (*pfnPredictMedian_align16)(BYTE *, const BYTE *, const BYTE *, DWORD);
	void (*pfnPredictMedianAndCount_align16)(BYTE *, const BYTE *, const BYTE *, DWORD, DWORD *);
	void (*pfnPredictMedianAndCount_align1)(BYTE *, const BYTE *, const BYTE *, DWORD, DWORD *);
	void (*pfnPredictLeftAndCount_align1)(BYTE *, const BYTE *, const BYTE *, DWORD *);
	void (*pfnRestoreMedian_align1)(BYTE *, const BYTE *, const BYTE *, DWORD);
	size_t (*pfnHuffmanEncode_align1)(BYTE *, const BYTE *, const BYTE *, const HUFFMAN_ENCODE_TABLE *);
	void (*pfnHuffmanDecode_align1)(BYTE *, BYTE *, const BYTE *, const HUFFMAN_DECODE_TABLE *);
	void (*pfnHuffmanDecodeAndAccum_align1)(BYTE *, BYTE *, const BYTE *, const HUFFMAN_DECODE_TABLE *);
	void (*pfnConvertULY2ToBottomupRGB24)(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pYBegin, const BYTE *pUBegin, const BYTE *pVBegin, DWORD dwStride, DWORD dwDataStride);
	void (*pfnConvertULY2ToBottomupRGB32)(BYTE *pDstBegin, BYTE *pDstEnd, const BYTE *pYBegin, const BYTE *pUBegin, const BYTE *pVBegin, DWORD dwStride, DWORD dwDataStride);
	void (*pfnConvertBottomupRGB24ToULY2)(BYTE *pYBegin, BYTE *pUBegin, BYTE *pVBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride, DWORD dwDataStride);
	void (*pfnConvertBottomupRGB32ToULY2)(BYTE *pYBegin, BYTE *pUBegin, BYTE *pVBegin, const BYTE *pSrcBegin, const BYTE *pSrcEnd, DWORD dwStride, DWORD dwDataStride);
};

extern TUNEDFUNC tfn;

void InitializeTunedFunc(void);
