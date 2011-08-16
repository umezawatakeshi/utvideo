/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

#ifdef _WIN32
int UtVideoFormatToWindowsFormat(DWORD *biCompression, WORD *biBitCount, GUID *subtype, utvf_t utvf);
int WindowsFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount, REFGUID subtype);
int WindowsFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount);
int WindowsFormatToUtVideoFormat(utvf_t *utvf, REFGUID subtype);
#endif

#if 0
int UtVideoFormatToQuickTimeFormat(OSType *pixelFormat, utvf_t utvf);
int QuickTimeFormatToUtVideoFormat(utvf_t *utvf, OSType pixelFormat);
#endif
