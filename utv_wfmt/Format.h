/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

int UtVideoFormatToVCMFormat(DWORD *biCompression, WORD *biBitCount, utvf_t utvf);
int VCMFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount);

int UtVideoFormatToDirectShowFormat(GUID *subtype, utvf_t utvf);
int DirectShowFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount, REFGUID subtype);
int DirectShowFormatToUtVideoFormat(utvf_t *utvf, REFGUID subtype);
