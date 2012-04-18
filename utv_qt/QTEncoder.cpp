/*  */
/* $Id$ */


#include "stdafx.h"
#include "utvideo.h"
#include "QTEncoder.h"
#include "Codec.h"
#include "Format.h"

extern "C" pascal ComponentResult QTEncoderComponentDispatch(ComponentParameters *params, CQTEncoder *glob);

#define IMAGECODEC_BASENAME()		QTEncoder
#define IMAGECODEC_GLOBALS()		CQTEncoder *storage
#define CALLCOMPONENT_BASENAME()	IMAGECODEC_BASENAME()
#define CALLCOMPONENT_GLOBALS()		IMAGECODEC_GLOBALS()

#define COMPONENT_DISPATCH_FILE		"QTEncoderDispatch.h"
#define COMPONENT_SELECT_PREFIX()	kImageCodec
#define COMPONENT_UPP_PREFIX()		uppImageCodec

#define GET_DELEGATE_COMPONENT()	(storage->delegateComponent)

#include <CoreServices/Components.k.h>
#include <QuickTime/ImageCodec.k.h>
#include <QuickTime/ComponentDispatchHelper.c>

//FILE *fp = NULL;

#define FCC4PRINTF(x) \
	((char)(x >> 24)), \
	((char)(x >> 16)), \
	((char)(x >>  8)), \
	((char)(x >>  0))
/*
extern "C" pascal ComponentResult QTEncoderComponentDispatchStub(ComponentParameters *params, CQTEncoder *glob)
{
	if (fp == NULL)
	{
		fp = fopen("/Users/umezawa/utv.log", "wt");
		setvbuf(fp, NULL, _IONBF, 0);
	}

	char fcc[5];
	if (params->what == kComponentOpenSelect)
		sprintf(fcc, "????");
	else
		sprintf(fcc, "%c%c%c%c", FCC4PRINTF(glob->componentSubType));

	if (params->what != kImageCodecSetSettingsSelect)
		fprintf(fp, "fcc=%s QTEncoderComponentDispatchStub what=%d size=%d\n", fcc, params->what, params->paramSize);

	if (params->what == kComponentCanDoSelect) {
		ComponentResult e = QTEncoderComponentDispatch(params, glob);
		fprintf(fp, "fcc=%s QTEncoderCanDo what=%d(0x%x) ret=%ld\n", fcc, ((SInt16*)(params->params))[0], (unsigned int)((SInt16*)(params->params))[0], e);
		return e;
	} else {
		return QTEncoderComponentDispatch(params, glob);
	}
}*/

pascal ComponentResult QTEncoderOpen(CQTEncoder *glob, ComponentInstance self)
{
	OSErr err;
	
	glob = (CQTEncoder *)NewPtrClear(sizeof(CQTEncoder));
	if (glob == NULL)
		return memFullErr;
	
	err = QTCodecOpen(glob, self);
	if (err != noErr)
	{
		DisposePtr((Ptr)glob);
		return err;
	}
	
	return noErr;
}

pascal ComponentResult QTEncoderClose(CQTEncoder *glob, ComponentInstance self)
{
	if (glob != NULL)
	{
		QTCodecClose(glob, self);

		ICMCompressionSessionOptionsRelease(glob->sessionOptions);

		DisposePtr((Ptr)glob);
	}

	return noErr;
}

pascal ComponentResult QTEncoderVersion(CQTEncoder *glob)
{
	return QTCodecVersion(glob);
}

pascal ComponentResult QTEncoderTarget(CQTEncoder *glob, ComponentInstance target)
{
	return QTCodecTarget(glob, target);
}

pascal ComponentResult QTEncoderGetCodecInfo(CQTEncoder *glob, CodecInfo *info)
{
	return QTCodecGetCodecInfo(glob, info);
}

pascal ComponentResult QTEncoderGetSettings(CQTEncoder *glob, Handle settings)
{
	size_t cbState;

	cbState = glob->codec->GetStateSize();
	SetHandleSize(settings, cbState);
	glob->codec->GetState(*settings, cbState);

	return noErr;
}

pascal ComponentResult QTEncoderSetSettings(CQTEncoder *glob, Handle settings)
{
	glob->codec->SetState(*settings, GetHandleSize(settings));

	return noErr;
}

pascal ComponentResult QTEncoderPrepareToCompressFrames(CQTEncoder *glob, ICMCompressorSessionRef session, ICMCompressionSessionOptionsRef sessionOptions, ImageDescriptionHandle imageDescription, void *reserved, CFDictionaryRef *compressorPixelBufferAttributesOut)
{
//	fprintf(fp, "QTEncoderPrepareToCompressFrames(glob=%p, session=%p, sessionOptions=%p, imageDescription=%p, reserved=%p, compressorPixelBufferAttributesOut=%p)\n",
//			glob, session, sessionOptions, imageDescription, reserved, compressorPixelBufferAttributesOut);	
	glob->session = session;

	ICMCompressionSessionOptionsRelease(glob->sessionOptions);
	glob->sessionOptions = sessionOptions;
	ICMCompressionSessionOptionsRetain(glob->sessionOptions);

	size_t imgDescExtSize;
	Handle imgDescExt;

	imgDescExtSize = glob->codec->EncodeGetExtraDataSize();
	imgDescExt = NewHandle(imgDescExtSize);
	glob->codec->EncodeGetExtraData(*imgDescExt, imgDescExtSize, /* XXX */ UTVF_INVALID, (*imageDescription)->width, (*imageDescription)->height, /* XXX */ CBGROSSWIDTH_NATURAL);
	AddImageDescriptionExtension(imageDescription, imgDescExt, 'glbl');
	DisposeHandle(imgDescExt);

	CFMutableDictionaryRef dic;
	CFMutableArrayRef array;
	CFNumberRef num;
	SInt32 n;

	dic = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

	array = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
	CFDictionaryAddValue(dic, kCVPixelBufferPixelFormatTypeKey, array);
	{
		const utvf_t *utvf;
		OSType ost;

		for (utvf = glob->codec->GetEncoderInputFormat(); *utvf; utvf++)
		{
			if (UtVideoFormatToQuickTimeFormat(&ost, *utvf) != 0)
				continue;
			n = ost;
			num = CFNumberCreate(NULL, kCFNumberSInt32Type, &n);
			CFArrayAppendValue(array, num);
			CFRelease(num);
		}
	}
	CFRelease(array);

	n = (*imageDescription)->width;
	num = CFNumberCreate(NULL, kCFNumberSInt32Type, &n);
	CFDictionaryAddValue(dic, kCVPixelBufferWidthKey, num);
	CFRelease(num);

	n = (*imageDescription)->height;
	num = CFNumberCreate(NULL, kCFNumberSInt32Type, &n);
	CFDictionaryAddValue(dic, kCVPixelBufferWidthKey, num);
	CFRelease(num);

	*compressorPixelBufferAttributesOut = dic;

	(*imageDescription)->cType = glob->componentSubType;

	return noErr;
}

pascal ComponentResult QTEncoderEncodeFrame(CQTEncoder *glob, ICMCompressorSourceFrameRef sourceFrame, UInt32 flags)
{
//	fprintf(fp, "QTEncoderEncodeFrame(glob=%p, sourceFrame=%p, flags=%lu)\n", glob, sourceFrame, flags);

	ICMMutableEncodedFrameRef encoded = NULL;
	MediaSampleFlags mediaSampleFlags;
	ComponentResult err;
	CVPixelBufferRef sourcePixelBuffer = NULL;
	size_t encodedSize;
	bool keyFrame;
	const void *srcPtr;
	void *dstPtr;
	OSType srcType;
	utvf_t utvf;
	size_t rowBytes;
	unsigned int width, height;

	sourcePixelBuffer = ICMCompressorSourceFrameGetPixelBuffer(sourceFrame);
	CVPixelBufferLockBaseAddress(sourcePixelBuffer, 0);
	srcType = CVPixelBufferGetPixelFormatType(sourcePixelBuffer);
//	fprintf(fp, "srctype=%08lx\n", srcType);
	if (QuickTimeFormatToUtVideoFormat(&utvf, srcType) != 0)
	{
		CVPixelBufferUnlockBaseAddress(sourcePixelBuffer, 0);
		return paramErr;
	}
//	fprintf(fp, "utvf=%08x\n", utvf);
//	fprintf(fp, "width=%zu height=%zu row=%zu\n",CVPixelBufferGetWidth(sourcePixelBuffer), CVPixelBufferGetHeight(sourcePixelBuffer), CVPixelBufferGetBytesPerRow(sourcePixelBuffer));
	width = (unsigned int)CVPixelBufferGetWidth(sourcePixelBuffer);
	height = (unsigned int)CVPixelBufferGetHeight(sourcePixelBuffer);
	rowBytes = CVPixelBufferGetBytesPerRow(sourcePixelBuffer);
	err = glob->codec->EncodeBegin(utvf, width, height, rowBytes);

//	fprintf(fp, "a %ld\n", err);
	err = ICMEncodedFrameCreateMutable(glob->session, sourceFrame, glob->codec->EncodeGetOutputSize(utvf, width, height, rowBytes), &encoded);
//	fprintf(fp, "b %ld\n", err);
	dstPtr = ICMEncodedFrameGetDataPtr(encoded);
	srcPtr = CVPixelBufferGetBaseAddress(sourcePixelBuffer);
//	fprintf(fp, "src=%p dst=%p\n", srcPtr, dstPtr);
	encodedSize = glob->codec->EncodeFrame(dstPtr, &keyFrame, srcPtr);
//	fprintf(fp, "b1 encodedSize=%zu\n", encodedSize);
	err = ICMEncodedFrameSetDataSize(encoded, encodedSize);
//	fprintf(fp, "c %ld\n", err);
	mediaSampleFlags = 0;
	if (keyFrame)
		mediaSampleFlags |= mediaSampleDoesNotDependOnOthers;
	err = ICMEncodedFrameSetMediaSampleFlags(encoded, mediaSampleFlags);
//	fprintf(fp, "d %ld\n", err);
	err = ICMCompressorSessionEmitEncodedFrame(glob->session, encoded, 1, &sourceFrame);
//	fprintf(fp, "e %ld\n", err);

	CVPixelBufferUnlockBaseAddress(sourcePixelBuffer, 0);
	glob->codec->EncodeEnd();
	ICMEncodedFrameRelease(encoded);
	return noErr;
}

pascal ComponentResult QTEncoderCompleteFrame(CQTEncoder *glob, ICMCompressorSourceFrameRef sourceFrame, UInt32 flags)
{
	return noErr;
}
