/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

#ifndef FCC
#define FCC(fcc) ( \
	(((uint32_t)(fcc) & 0x000000ff) << 24) | \
	(((uint32_t)(fcc) & 0x0000ff00) <<  8) | \
	(((uint32_t)(fcc) & 0x00ff0000) >>  8) | \
	(((uint32_t)(fcc) & 0xff000000) >> 24))
#endif

#define UNFCC(fcc) ( \
	(((uint32_t)(fcc) & 0x000000ff) << 24) | \
	(((uint32_t)(fcc) & 0x0000ff00) <<  8) | \
	(((uint32_t)(fcc) & 0x00ff0000) >>  8) | \
	(((uint32_t)(fcc) & 0xff000000) >> 24))

int UtVideoFormatToVCMFormat(DWORD *biCompression, WORD *biBitCount, utvf_t utvf);
int VCMFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount);

int UtVideoFormatToDirectShowFormat(GUID *subtype, utvf_t utvf);
int DirectShowFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount, REFGUID subtype);
int DirectShowFormatToUtVideoFormat(utvf_t *utvf, REFGUID subtype);

int UtVideoFormatToMediaFoundationFormat(GUID *subtype, bool *bBottomup, utvf_t utvf);
int MediaFoundationFormatToUtVideoFormat(utvf_t *utvf, REFGUID subtype, bool bBottomup);
