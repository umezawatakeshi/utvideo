/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

#ifdef _WIN32
__declspec(dllexport) int UtVideoFormatToWindowsFormat(DWORD *biCompression, WORD *biBitCount, GUID *subtype, utvf_t utvf);
__declspec(dllexport) int WindowsFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount, REFGUID subtype);
__declspec(dllexport) int WindowsFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount);
__declspec(dllexport) int WindowsFormatToUtVideoFormat(utvf_t *utvf, REFGUID subtype);
#endif

#if 0
int UtVideoFormatToQuickTimeFormat(OSType *pixelFormat, utvf_t utvf);
int QuickTimeFormatToUtVideoFormat(utvf_t *utvf, OSType pixelFormat);
#endif
