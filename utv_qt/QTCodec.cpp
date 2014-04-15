/*  */
/* $Id$ */


#include "stdafx.h"
#include "utvideo.h"
#include "QTCodec.h"
#include "Codec.h"
#include "Format.h"

pascal ComponentResult QTCodecOpen(CQTCodec *glob, ComponentInstance self)
{
	ComponentDescription cd;
	utvf_t utvf;

	GetComponentInfo((Component)self, &cd, 0, 0, 0);

	SetComponentInstanceStorage(self, (Handle)glob);
	glob->self = self;
	glob->target = self;
	glob->componentSubType = cd.componentSubType;
	QuickTimeFormatToUtVideoFormat(&utvf, glob->componentSubType);
	glob->codec = CCodec::CreateInstance(utvf, "QT");
	glob->mutex = new CMutex;
	OpenADefaultComponent(cd.componentType, kBaseCodecType, &glob->delegateComponent);
	ComponentSetTarget(glob->delegateComponent, self);

	return noErr;
}

pascal ComponentResult QTCodecClose(CQTCodec *glob, ComponentInstance self)
{
	if (glob->delegateComponent != NULL)
		CloseComponent(glob->delegateComponent);
	delete glob->mutex;
	CCodec::DeleteInstance(glob->codec);

	return noErr;
}

pascal ComponentResult QTCodecVersion(CQTCodec *glob)
{
	CMutexLock lock(glob->mutex);

	return 0x1000000;
}

pascal ComponentResult QTCodecTarget(CQTCodec *glob, ComponentInstance target)
{
	CMutexLock lock(glob->mutex);

	glob->target = target;

	return noErr;
}

pascal ComponentResult QTCodecGetCodecInfo(CQTCodec *glob, CodecInfo *info)
{
	CMutexLock lock(glob->mutex);

	ComponentResult err;
	CodecInfo **tempCodecInfo;
	char name[sizeof(info->typeName)];

	err = GetComponentResource((Component)glob->self, codecInfoResourceType, 256, (Handle *)&tempCodecInfo);
	if (err != noErr)
		return err;

	*info = **tempCodecInfo;
	DisposeHandle((Handle)tempCodecInfo);

	glob->codec->GetLongFriendlyName(name, sizeof(name));
	{
		char *src;
		unsigned char *dst;
		info->typeName[0] = (unsigned char)strlen(name);
		src = name;
		dst = info->typeName + 1;
		for (; *src; src++, dst++)
			*dst = *src;
	}

	return noErr;
}
