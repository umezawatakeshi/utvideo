/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

int UtVideoFormatToVCMFormat(DWORD *biCompression, WORD *biBitCount, utvf_t utvf);
int VCMFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount);

int UtVideoFormatToDirectShowFormat(GUID *subtype, utvf_t utvf);
int DirectShowFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount, REFGUID subtype);
int DirectShowFormatToUtVideoFormat(utvf_t *utvf, REFGUID subtype);

int UtVideoFormatToMediaFoundationFormat(GUID *subtype, utvf_t utvf);
int MediaFoundationFormatToUtVideoFormat(utvf_t *utvf, REFGUID subtype, bool bBottomup);
