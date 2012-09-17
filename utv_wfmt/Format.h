/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

int UtVideoFormatToWindowsFormat(DWORD *biCompression, WORD *biBitCount, utvf_t utvf);
int UtVideoFormatToWindowsFormat(GUID *subtype, utvf_t utvf);
int WindowsFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount, REFGUID subtype);
int WindowsFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount);
int WindowsFormatToUtVideoFormat(utvf_t *utvf, REFGUID subtype);
