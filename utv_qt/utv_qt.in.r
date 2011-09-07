/*  */
/* $Id$ */

resource 'strn' (SUBTYPE_RID) {
	"UtVideo (" SUBTYPE_STR ")"
};

resource 'stri' (SUBTYPE_RID) {
	"Ut Video Codec " SUBTYPE_STRI "(" SUBTYPE_STR ") QT"
};

resource 'thng' (SUBTYPE_RID) {
	decompressorComponentType,			// Type                 
	SUBTYPE_VAL,						// SubType
	'Umzw',								// Manufacturer
	0,									// - use componentHasMultiplePlatforms
	0,
	0,
	0,
	'strn',								// Name Type
	SUBTYPE_RID,						// Name ID
	'stri',								// Info Type
	SUBTYPE_RID,						// Info ID
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

#undef SUBTYPE
#undef SUBTYPE_VAL
#undef SUBTYPE_RID
#undef SUBTYPE_STRI
