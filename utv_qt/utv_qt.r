/*  */
/* $Id$ */

#define thng_RezTemplateVersion 1
#define cfrg_RezTemplateVersion 1

#include <QuickTime/QuickTimeComponents.r>
#include <QuickTime/ImageCodec.r>

resource 'cdci' (256) {
	"Ut Video Codec Suite",				// Name
	1,									// Version
	1,									// Revision level
	'Umzw',								// Manufacturer
	codecInfoDoes32,					// Decompression Flags
	0,									// Compression Flags
	codecInfoDepth32,					// Format Flags
	128,								// Compression Accuracy
	128,								// Decomression Accuracy
	200,								// Compression Speed
	200,								// Decompression Speed
	128,								// Compression Level
	0,									// Reserved
	1,									// Minimum Height
	1,									// Minimum Width
	0,									// Decompression Pipeline Latency
	0,									// Compression Pipeline Latency
	0									// Private Data
};

resource 'dlle' (256) {
	"QTDecoderComponentDispatch"
};

resource 'dlle' (257) {
	"QTEncoderComponentDispatch"
};

#define SUBTYPE      ULRA
#define SUBTYPE_VAL  'ULRA'
#define SUBTYPE_STR  "ULRA"
#define SUBTYPE_RID  256
#define SUBTYPE_INFO "RGBA"
#include "utv_qt.in.r"

#define SUBTYPE      ULRG
#define SUBTYPE_VAL  'ULRG'
#define SUBTYPE_STR  "ULRG"
#define SUBTYPE_RID  257
#define SUBTYPE_INFO "RGB"
#include "utv_qt.in.r"

#define SUBTYPE      ULY0
#define SUBTYPE_VAL  'ULY0'
#define SUBTYPE_STR  "ULY0"
#define SUBTYPE_RID  258
#define SUBTYPE_INFO "YUV420 BT.601"
#include "utv_qt.in.r"

#define SUBTYPE      ULY2
#define SUBTYPE_VAL  'ULY2'
#define SUBTYPE_STR  "ULY2"
#define SUBTYPE_RID  259
#define SUBTYPE_INFO "YUV422 BT.601"
#include "utv_qt.in.r"

#define SUBTYPE      ULH0
#define SUBTYPE_VAL  'ULH0'
#define SUBTYPE_STR  "ULH0"
#define SUBTYPE_RID  260
#define SUBTYPE_INFO "YUV420 BT.709"
#include "utv_qt.in.r"

#define SUBTYPE      ULH2
#define SUBTYPE_VAL  'ULH2'
#define SUBTYPE_STR  "ULH2"
#define SUBTYPE_RID  261
#define SUBTYPE_INFO "YUV422 BT.709"
#include "utv_qt.in.r"
