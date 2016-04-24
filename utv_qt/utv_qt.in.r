/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#define SUBTYPE_FMTN "UtVideo " SUBTYPE_INFO

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
	0x00040001,							// Version
	componentHasMultiplePlatforms +		// Registration Flags 
	componentDoAutoVersion,				// Registration Flags
	0,									// Resource ID of Icon Family
	{
		codecInfoDoes32,
		'dlle',
		256,
		platformTarget,
	},
	'thnr', SUBTYPE_RID;
};

resource 'cpix' (SUBTYPE_RID) {
	SUBTYPE_DEC
};

resource 'thnr' (SUBTYPE_RID) {
	{
		'cpix', 1, 0,
		'cpix', SUBTYPE_RID, 0,
	}
};

resource 'stri' (SUBTYPE_RID + 256) {
	"Compresses video stored in " SUBTYPE_FMTN " format."
};

resource 'thng' (SUBTYPE_RID + 256) {
	compressorComponentType,			// Type
	SUBTYPE_VAL,						// SubType
	'Umzw',								// Manufacturer
	0,									// - use componentHasMultiplePlatforms
	0,
	0,
	0,
	'strn', SUBTYPE_RID,				// Name Type & ID
	'stri', SUBTYPE_RID + 256,			// Info Type & ID
	0, 0,								// Icon Type & ID
	0x00040001,							// Version
	componentHasMultiplePlatforms +		// Registration Flags 
	componentDoAutoVersion,				// Registration Flags
	0,									// Resource ID of Icon Family
	{
		codecInfoDoes32,
		'dlle',
		257,
		platformTarget,
	},
	'thnr', SUBTYPE_RID + 256;
};

resource 'cpix' (SUBTYPE_RID + 256) {
	SUBTYPE_ENC
};

resource 'thnr' (SUBTYPE_RID + 256) {
	{
		'cpix', 1, 0,
		'cpix', SUBTYPE_RID + 256, 0,
	}
};

#undef SUBTYPE_FMTN

#undef SUBTYPE
#undef SUBTYPE_VAL
#undef SUBTYPE_RID
#undef SUBTYPE_INFO
#undef SUBTYPE_ENC
#undef SUBTYPE_DEC
