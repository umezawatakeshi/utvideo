/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"

struct FRAME_METRIC
{
	int nPlanes;
	int nMacroPixelWidth;
	int nMacroPixelHeight;
	int nWidthStep;
	int nHeightStep;
	int nStripes;
	size_t cbTotalSize;
	int nLines[4];
	int nStripeLines[4];
	size_t cbLineWidth[4];
	ssize_t scbLineStride[4];
	ssize_t scbStripeStride[4];
	size_t cbPlaneOffset[4];
	ssize_t cbFirstLineOffset[4];
};

int CalcRawFrameMetric(FRAME_METRIC* pfm, utvf_t rawfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth);
void FixStripeValues(FRAME_METRIC* pfm);

class CCodecBase :
	public CCodec
{
protected:
	const char *const m_pszTinyName;
	const char *const m_pszInterfaceName;

	FRAME_METRIC m_fmRaw;

protected:
	CCodecBase(const char *pszTinyName, const char *pszInterfaceName);
	virtual ~CCodecBase(void);

public:
	virtual const char *GetTinyName(void);
	virtual void GetShortFriendlyName(char *pszName, size_t cchName);
	virtual void GetShortFriendlyName(wchar_t *pszName, size_t cchName);
	virtual void GetLongFriendlyName(char *pszName, size_t cchName) = 0;
	virtual void GetLongFriendlyName(wchar_t *pszName, size_t cchName);
	virtual int SetState(const void *pState, size_t cb);

	virtual int EncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t* cbGrossWidth);
	virtual int EncodeEnd(void);
	virtual int EncodeQuery(utvf_t infmt, unsigned int width, unsigned int height);
	virtual int DecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth, const void *pExtraData, size_t cbExtraData);
	virtual int DecodeEnd(void);
	virtual size_t DecodeGetOutputSize(utvf_t outfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth);
	virtual int DecodeQuery(utvf_t outfmt, unsigned int width, unsigned int height, const void *pExtraData, size_t cbExtraData);

protected:
	int LoadConfig(void);
	int SaveConfig(void);
	int InternalSetStateWrapper(const void *pState, size_t cb);
	int CalcRawFrameMetric(utvf_t rawfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth);

	virtual int InternalSetState(const void *pState, size_t cb) = 0;

	virtual int InternalEncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t* cbGrossWidth) = 0;
	virtual int InternalEncodeEnd(void) = 0;
	virtual int InternalEncodeQuery(utvf_t infmt, unsigned int width, unsigned int height) = 0;
	virtual int InternalDecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t* cbGrossWidth, const void *pExtraData, size_t cbExtraData) = 0;
	virtual int InternalDecodeEnd(void) = 0;
	virtual int InternalDecodeQuery(utvf_t outfmt, unsigned int width, unsigned int height, const void *pExtraData, size_t cbExtraData) = 0;
};
