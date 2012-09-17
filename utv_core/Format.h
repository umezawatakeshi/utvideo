/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

#ifdef _WIN32
DLLEXPORT int UtVideoFormatToWindowsFormat(DWORD *biCompression, WORD *biBitCount, utvf_t utvf);
DLLEXPORT int UtVideoFormatToWindowsFormat(GUID *subtype, utvf_t utvf);
DLLEXPORT int WindowsFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount, REFGUID subtype);
DLLEXPORT int WindowsFormatToUtVideoFormat(utvf_t *utvf, DWORD biCompression, WORD biBitCount);
DLLEXPORT int WindowsFormatToUtVideoFormat(utvf_t *utvf, REFGUID subtype);
#endif
