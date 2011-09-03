/*  */
/* $Id$ */

#define TARGET_REZ_CARBON_MACHO 1
#define thng_RezTemplateVersion 1
#define cfrg_RezTemplateVersion 1

#include <Carbon/Carbon.r>
#include <QuickTime/QuickTime.r>

resource 'cdci' (256) {
	"Ut Video Codec Suite",				// Name
	1,									// Version
	1,									// Revision level
	'Umzw',								// Manufacturer
	codecInfoDepth24,					// Decompression Flags
	0,									// Compression Flags
	codecInfoDepth24,					// Format Flags
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

resource 'strn' (256) {
	"UtVideo (ULRG)"
};

resource 'stri' (256) {
	"Ut Video Codec RGB (ULRG) QT"
};

resource 'dlle' (256) {
	"QTDecoderComponentDispatch"
};

resource 'thng' (256) {
	decompressorComponentType,			// Type                 
	'ULRG',								// SubType
	'Umzw',								// Manufacturer
	0,									// - use componentHasMultiplePlatforms
	0,
	0,
	0,
	'strn',								// Name Type
	256,								// Name ID
	'stri',								// Info Type
	256,								// Info ID
	0,									// Icon Type
	0,									// Icon ID
	0x1000000,							// Version
	componentHasMultiplePlatforms +		// Registration Flags 
	componentDoAutoVersion,				// Registration Flags
	0,									// Resource ID of Icon Family
	{
		codecInfoDepth24,
		'dlle',
		256,
		platformIA32NativeEntryPoint,
	};
};
