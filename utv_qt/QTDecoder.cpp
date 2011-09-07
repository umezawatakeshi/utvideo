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
	glob->beginBandDone = false;
	glob->codec = CCodec::CreateInstance(utvf, "QT");
	glob->mutex = new CMutex;
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
		delete glob->mutex;
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
	CMutexLock lock(glob->mutex);

	return 0x1000000;
}

pascal ComponentResult QTDecoderTarget(CQTDecoder *glob, ComponentInstance target)
{
	CMutexLock lock(glob->mutex);

	glob->target = target;

	return noErr;
}


pascal ComponentResult QTDecoderGetCodecInfo(CQTDecoder *glob, CodecInfo *info)
{
	CMutexLock lock(glob->mutex);

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
	CMutexLock lock(glob->mutex);

	CodecCapabilities *cap = param->capabilities;

	param->wantedDestinationPixelTypes = glob->wantedDestinationPixelTypes;

	return noErr;
}

pascal ComponentResult QTDecoderInitialize(CQTDecoder *glob, ImageSubCodecDecompressCapabilities *cap)
{
	CMutexLock lock(glob->mutex);

	cap->decompressRecordSize = sizeof(bool);

	return noErr;
}

pascal ComponentResult QTDecoderBeginBand(CQTDecoder *glob, CodecDecompressParams *param, ImageSubCodecDecompressRecord *drp, long flags)
{
	CMutexLock lock(glob->mutex);

	int i;
	long c;
	Handle imgDescExt;
	size_t imgDescExtSize;
	utvf_t outfmt;

	if (glob->beginBandDone)
	{
		*(bool *)drp->userDecompressRecord = true;
		return paramErr;
	}
	glob->beginBandDone = true;
	*(bool *)drp->userDecompressRecord = false;
	
#define SIZEOF_BITMAPINFOHEADER 40
	
	if (QuickTimeFormatToUtVideoFormat(&outfmt, param->dstPixMap.pixelFormat) != 0)
		return paramErr;
	
	imgDescExt = NULL;
	if (GetImageDescriptionExtension(param->imageDescription, &imgDescExt, 'strf', 1) != noErr)
		return paramErr;
	imgDescExtSize = GetHandleSize(imgDescExt);
	if (imgDescExtSize < SIZEOF_BITMAPINFOHEADER)
	{
		DisposeHandle(imgDescExt);
		return paramErr;
	}
	glob->codec->DecodeBegin(outfmt, (*param->imageDescription)->width, (*param->imageDescription)->height, drp->rowBytes,
							 ((char *)(*imgDescExt)) + SIZEOF_BITMAPINFOHEADER, imgDescExtSize - SIZEOF_BITMAPINFOHEADER);
	DisposeHandle(imgDescExt);

	return noErr;
}

pascal ComponentResult QTDecoderDrawBand(CQTDecoder *glob, ImageSubCodecDecompressRecord *drp)
{
	CMutexLock lock(glob->mutex);

	glob->codec->DecodeFrame(drp->baseAddr, drp->codecData, true /* XXX */);

	return noErr;
}

pascal ComponentResult QTDecoderEndBand(CQTDecoder *glob, ImageSubCodecDecompressRecord *drp, OSErr result, long flags)
{
	CMutexLock lock(glob->mutex);

	if (*(bool *)drp->userDecompressRecord)
		return noErr;

	glob->codec->DecodeEnd();
	glob->beginBandDone = false;

	return noErr;
}

pascal ComponentResult QTDecoderDecodeBand(CQTDecoder *glob, ImageSubCodecDecompressRecord *drp, unsigned long flags)
{
	return noErr;
}
