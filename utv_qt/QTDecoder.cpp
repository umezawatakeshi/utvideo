/*  */
/* $Id$ */


#include "stdafx.h"
#include "utvideo.h"
#include "QTDecoder.h"
#include "Codec.h"
#include "Format.h"

extern "C" pascal ComponentResult QTDecoderComponentDispatch(ComponentParameters *params, CQTDecoder *glob);

#define IMAGECODEC_BASENAME()		QTDecoder
#define IMAGECODEC_GLOBALS()		CQTDecoder *storage
#define CALLCOMPONENT_BASENAME()	IMAGECODEC_BASENAME()
#define CALLCOMPONENT_GLOBALS()		IMAGECODEC_GLOBALS()

#define COMPONENT_DISPATCH_FILE		"QTDecoderDispatch.h"
#define COMPONENT_SELECT_PREFIX()	kImageCodec
#define COMPONENT_UPP_PREFIX()		uppImageCodec

#define GET_DELEGATE_COMPONENT()	(storage->delegateComponent)

#include <CoreServices/Components.k.h>
#include <QuickTime/ImageCodec.k.h>
#include <QuickTime/ComponentDispatchHelper.c>

pascal ComponentResult QTDecoderOpen(CQTDecoder *glob, ComponentInstance self)
{
	ComponentDescription descout;
	utvf_t utvf;

	GetComponentInfo((Component)self, &descout, 0, 0, 0);

	glob = (CQTDecoder *)NewPtrClear(sizeof(CQTDecoder));
	SetComponentInstanceStorage(self, (Handle)glob);
	glob->self = self;
	glob->target = self;
	glob->componentSubType = descout.componentSubType;
	QuickTimeFormatToUtVideoFormat(&utvf, glob->componentSubType);
	glob->codec = CCodec::CreateInstance(utvf, "QT");
	OpenADefaultComponent(decompressorComponentType, kBaseCodecType, &glob->delegateComponent);
	ComponentSetTarget(glob->delegateComponent, self);

	{
		const utvf_t *p;
		OSType *post;
		int n;

		p = glob->codec->GetDecoderOutputFormat();
		n = 0;
		while (*p)
		{
			n++;
			p++;
		}

		glob->wantedDestinationPixelTypes = (OSType **)NewHandleClear(sizeof(OSType) * (n + 1));
		post = *glob->wantedDestinationPixelTypes;
		for (p = glob->codec->GetDecoderOutputFormat(); *p; p++)
		{
			if (UtVideoFormatToQuickTimeFormat(post, *p) == 0)
				post++;
		}
		*post = 0;
	}

	return noErr;
}

pascal ComponentResult QTDecoderClose(CQTDecoder *glob, ComponentInstance self)
{
	if (glob != NULL)
	{
		CCodec::DeleteInstance(glob->codec);
		if (glob->wantedDestinationPixelTypes != NULL)
			DisposeHandle((Handle)glob->wantedDestinationPixelTypes);
		if (glob->delegateComponent != NULL)
			CloseComponent(glob->delegateComponent);
		DisposePtr((Ptr)glob);
	}

	return noErr;
}

pascal ComponentResult QTDecoderVersion(CQTDecoder *glob)
{
	return 0x1000000;
}

pascal ComponentResult QTDecoderTarget(CQTDecoder *glob, ComponentInstance target)
{
	glob->target = target;

	return noErr;
}


pascal ComponentResult QTDecoderGetCodecInfo(CQTDecoder *glob, CodecInfo *info)
{
	ComponentResult err;
	CodecInfo **tempCodecInfo;

	err = GetComponentResource((Component)glob->self, codecInfoResourceType, 256, (Handle *)&tempCodecInfo);
	if (err == noErr)
	{
		*((CodecInfo *)info) = **tempCodecInfo;
		DisposeHandle((Handle)tempCodecInfo);
	}
	return noErr;
}

pascal ComponentResult QTDecoderPreflight(CQTDecoder *glob, CodecDecompressParams *param)
{
	CodecCapabilities *cap = param->capabilities;

	param->wantedDestinationPixelTypes = glob->wantedDestinationPixelTypes;

	return noErr;
}

pascal ComponentResult QTDecoderInitialize(CQTDecoder *glob, ImageSubCodecDecompressCapabilities *cap)
{
	return noErr;
}

pascal ComponentResult QTDecoderBeginBand(CQTDecoder *glob, CodecDecompressParams *param, ImageSubCodecDecompressRecord *drp, long flags)
{
	int i;
	long c;
	Handle imgDescExt;

	CountImageDescriptionExtensionType(param->imageDescription, 'strf', &c);

	imgDescExt = NULL;
	GetImageDescriptionExtension(param->imageDescription, &imgDescExt, 'strf', 1);
	DisposeHandle(imgDescExt);
	return noErr;
}

pascal ComponentResult QTDecoderDrawBand(CQTDecoder *glob, ImageSubCodecDecompressRecord *drp)
{
	unsigned char *p = (unsigned char *)drp->baseAddr;

	for (int i = 0; i < 644*200; i++)
	{
		p[i*4    ] = 0; // A?
		p[i*4 + 1] = 255; // R
		p[i*4 + 2] = 0; // G
		p[i*4 + 3] = 0; // B
	}

	return noErr;
}

pascal ComponentResult QTDecoderEndBand(CQTDecoder *glob, ImageSubCodecDecompressRecord *drp, OSErr result, long flags)
{
	return noErr;
}

pascal ComponentResult QTDecoderDecodeBand(CQTDecoder *glob, ImageSubCodecDecompressRecord *drp, unsigned long flags)
{
	return noErr;
}
