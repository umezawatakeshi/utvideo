/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once
#include "Codec.h"
#include "CodecBase.h"
#include "FrameBuffer.h"
#include "Thread.h"
#include "HuffmanCode.h"


class CUTRDCodec :
	public CCodecBase
{
public:
	static const utvf_t m_utvfCodec = UTVF_UTRD;

private:
	static const utvf_t m_utvfEncoderInput[];
	static const utvf_t m_utvfDecoderOutput[];
	static const utvf_t m_utvfCompressed[];

protected:
	utvf_t m_utvfRaw;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	const void *m_pInput;
	void *m_pOutput;

	CFrameBuffer *m_pCurFrame;
	CFrameBuffer *m_pMedianPredicted;

	CFrameBuffer *m_pRestoredFrame;
	CFrameBuffer *m_pDecodedFrame;

public:
	CUTRDCodec(const char *pszInterfaceName);
	virtual ~CUTRDCodec(void) {}
	static CCodec *CreateInstance(const char *pszInterfaceName) { return new CUTRDCodec(pszInterfaceName); }

public:
	virtual void GetLongFriendlyName(char *pszName, size_t cchName);
	virtual bool IsTemporalCompressionSupported(void) { return false; }

#ifdef _WIN32
	virtual INT_PTR Configure(HWND hwnd);
#endif

	virtual size_t GetStateSize(void);
	virtual int GetState(void *pState, size_t cb);

	virtual int InternalEncodeBegin(utvf_t infmt, unsigned int width, unsigned int height, size_t cbGrossWidth);
	virtual size_t EncodeFrame(void *pOutput, bool *pbKeyFrame, const void *pInput);
	virtual int InternalEncodeEnd(void);
	virtual size_t EncodeGetExtraDataSize(void);
	virtual int EncodeGetExtraData(void *pExtraData, size_t cb, utvf_t infmt, unsigned int width, unsigned int height);
	virtual size_t EncodeGetOutputSize(utvf_t infmt, unsigned int width, unsigned int height);
	virtual int InternalEncodeQuery(utvf_t infmt, unsigned int width, unsigned int height);

	virtual int InternalDecodeBegin(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth, const void *pExtraData, size_t cbExtraData);
	virtual size_t DecodeFrame(void *pOutput, const void *pInput);
	virtual int DecodeGetFrameType(bool *pbKeyFrame, const void *pInput);
	virtual int InternalDecodeEnd(void);
	virtual size_t DecodeGetOutputSize(utvf_t outfmt, unsigned int width, unsigned int height, size_t cbGrossWidth);
	virtual int InternalDecodeQuery(utvf_t outfmt, unsigned int width, unsigned int height, const void *pExtraData, size_t cbExtraData);

public:
	virtual const utvf_t *GetEncoderInputFormat(void) { return m_utvfEncoderInput; }
	virtual const utvf_t *GetDecoderOutputFormat(void) { return m_utvfDecoderOutput; }
	virtual const utvf_t *GetCompressedFormat(void) { return m_utvfCompressed; }

protected:
	virtual int InternalSetState(const void *pState, size_t cb);

private:
};
