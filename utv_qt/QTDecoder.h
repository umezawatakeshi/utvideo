/*  */
/* $Id$ */

#pragma once

#include "QTCodec.h"
#include "Codec.h"
#include "Mutex.h"

struct CQTDecoder : public CQTCodec
{
	OSType **wantedDestinationPixelTypes;
	int beginBandDone;
};
