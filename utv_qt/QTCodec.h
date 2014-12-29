/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

#include "Codec.h"
#include "Mutex.h"

struct CQTCodec
{
	ComponentInstance self;
	ComponentInstance delegateComponent;
	ComponentInstance target;
	OSType componentSubType;
	CCodec *codec;
	CMutex *mutex;
};

pascal ComponentResult QTCodecOpen(CQTCodec *glob, ComponentInstance self);
pascal ComponentResult QTCodecClose(CQTCodec *glob, ComponentInstance self);
pascal ComponentResult QTCodecVersion(CQTCodec *glob);
pascal ComponentResult QTCodecTarget(CQTCodec *glob, ComponentInstance target);
pascal ComponentResult QTCodecGetCodecInfo(CQTCodec *glob, CodecInfo *info);
