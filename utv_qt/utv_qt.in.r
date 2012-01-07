/*  */
/* $Id$ */

#define SUBTYPE_FMTN "Ut Video Codec " SUBTYPE_INFO " (" SUBTYPE_STR ")"

resource 'strn' (SUBTYPE_RID) {
	SUBTYPE_FMTN " QT"
};

resource 'stri' (SUBTYPE_RID) {
	"Decompresses video stored in " SUBTYPE_FMTN " format."
};

resource 'thng' (SUBTYPE_RID) {
	decompressorComponentType,			// Type                 
	SUBTYPE_VAL,						// SubType
	'Umzw',								// Manufacturer
	0,									// - use componentHasMultiplePlatforms
	0,
	0,
	0,
	'strn', SUBTYPE_RID,				// Name Type & ID
	'stri', SUBTYPE_RID,				// Info Type & ID
	0, 0,								// Icon Type & ID
	0x1000000,							// Version
	componentHasMultiplePlatforms +		// Registration Flags 
	componentDoAutoVersion,				// Registration Flags
	0,									// Resource ID of Icon Family
	{
		codecInfoDoes32,
		'dlle',
		256,
		platformIA32NativeEntryPoint,
	};
};

#undef SUBTYPE_FMTN

#undef SUBTYPE
#undef SUBTYPE_VAL
#undef SUBTYPE_RID
#undef SUBTYPE_INFO
