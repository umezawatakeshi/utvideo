/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

#include "QTCodec.h"
#include "Codec.h"

struct CQTEncoder : public CQTCodec
{
	ICMCompressorSessionRef session;
	ICMCompressionSessionOptionsRef sessionOptions;
    bool encodeBegun;
};
