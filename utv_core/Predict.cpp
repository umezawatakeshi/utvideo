/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#include "stdafx.h"
#include "utvideo.h"
#include "Predict.h"
#include "TunedFunc.h"

template<bool DoCount>
void cpp_PredictCylindricalWrongMedianAndMayCount(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride, uint32_t pCountTable[][256])
{
	const uint8_t *p = pSrcBegin;
	uint8_t *q = pDst;

	// �ŏ��̃��C���̍ŏ��̃s�N�Z���� 0x80 ��\�����Ă����B
	*q = *p - 0x80;
	if (DoCount)
		pCountTable[0][*q]++;
	p++;
	q++;

	// �ŏ��̃��C���̎c��̃s�N�Z���� predict left �Ɠ����B
	for (; p < pSrcBegin + cbStride; p++, q++)
	{
		*q = *p - *(p - 1);
		if (DoCount)
			pCountTable[0][*q]++;
	}

	// 1 ���C�������Ȃ��ꍇ�͂����ŏI���B
	if (p == pSrcEnd)
		return;

	// ���̃��C���̍ŏ��̃s�N�Z���� predict above�B
	// �������Ă����ƃA�Z���u�����������ɏ������኱�ȒP�ɂȂ�B
	*q = *p - *(p - cbStride);
	if (DoCount)
		pCountTable[0][*q]++;
	p++;
	q++;

	// �c��̃s�N�Z���� predict median �̖{��
	for (; p < pSrcEnd; p++, q++)
	{
		*q = *p - median<uint8_t>(*(p - cbStride), *(p - 1), *(p - cbStride) + *(p - 1) - *(p - 1 - cbStride));
		if (DoCount)
			pCountTable[0][*q]++;
	}
}

void cpp_PredictCylindricalWrongMedianAndCount(uint8_t* pDst, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbStride, uint32_t pCountTable[][256])
{
	cpp_PredictCylindricalWrongMedianAndMayCount<true>(pDst, pSrcBegin, pSrcEnd, cbStride, pCountTable);
}

void cpp_PredictCylindricalWrongMedian(uint8_t* pDst, const uint8_t* pSrcBegin, const uint8_t* pSrcEnd, size_t cbStride)
{
	cpp_PredictCylindricalWrongMedianAndMayCount<false>(pDst, pSrcBegin, pSrcEnd, cbStride, NULL);
}

void cpp_RestoreCylindricalWrongMedian(uint8_t *pDst, const uint8_t *pSrcBegin, const uint8_t *pSrcEnd, size_t cbStride)
{
	const uint8_t *p = pSrcBegin;
	uint8_t *q = pDst;

	*q++ = *p++ + 0x80;

	for (; p < pSrcBegin + cbStride; p++, q++)
	{
		*q = *p + *(q - 1);
	}

	if (p == pSrcEnd)
		return;

	*q = *p + *(q - cbStride);
	p++;
	q++;

	for (; p < pSrcEnd; p++, q++)
	{
		*q = *p + median<uint8_t>(*(q - cbStride), *(q - 1), *(q - cbStride) + *(q - 1) - *(q - 1 - cbStride));
	}
}


template<int B>
void cpp_PredictCylindricalLeftAndCount(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd, uint32_t pCountTable[][1 << B])
{
	const symbol_t<B> *p = pSrcBegin;
	symbol_t<B> *q = pDst;

	*q = (*p - CSymbolBits<B>::midval) & CSymbolBits<B>::maskval;
	pCountTable[0][*q]++;
	p++;
	q++;

	// �c��̃s�N�Z���� predict left �̖{��
	for (; p < pSrcEnd; p++, q++)
	{
		*q = (*p - *(p-1)) & CSymbolBits<B>::maskval;
		pCountTable[0][*q]++;
	}
}

template void cpp_PredictCylindricalLeftAndCount<8>(symbol_t<8> *pDst, const symbol_t<8> *pSrcBegin, const symbol_t<8> *pSrcEnd, uint32_t pCountTable[][256]);
template void cpp_PredictCylindricalLeftAndCount<10>(symbol_t<10> *pDst, const symbol_t<10> *pSrcBegin, const symbol_t<10> *pSrcEnd, uint32_t pCountTable[][1024]);


template<int B>
void cpp_RestoreCylindricalLeft(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd)
{
	const symbol_t<B> *p = pSrcBegin;
	symbol_t<B> *q = pDst;

	*q = (*p + CSymbolBits<B>::midval) & CSymbolBits<B>::maskval;
	p++;
	q++;

	// �c��̃s�N�Z���� predict left �̖{��
	for (; p < pSrcEnd; p++, q++)
	{
		*q = (*(q - 1) + *p) & CSymbolBits<B>::maskval;
	}
}

template void cpp_RestoreCylindricalLeft<8>(symbol_t<8> *pDst, const symbol_t<8> *pSrcBegin, const symbol_t<8> *pSrcEnd);
template void cpp_RestoreCylindricalLeft<10>(symbol_t<10> *pDst, const symbol_t<10> *pSrcBegin, const symbol_t<10> *pSrcEnd);


template<int B, bool DoCount>
static inline void cpp_PredictPlanarGradientAndMayCount(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd, size_t cbStride, uint32_t pCountTable[][1 << B])
{
	const symbol_t<B>* p = pSrcBegin;
	symbol_t<B>* q = pDst;
	size_t nStride = cbStride / sizeof(symbol_t<B>);

	*q = (*p - CSymbolBits<B>::midval) & CSymbolBits<B>::maskval;
	if (DoCount)
		pCountTable[0][*q]++;
	p++;
	q++;

	for (; p < pSrcBegin + nStride; p++, q++)
	{
		*q = (*p - *(p - 1)) & CSymbolBits<B>::maskval;
		if (DoCount)
			pCountTable[0][*q]++;
	}

	for (auto pp = pSrcBegin + nStride; pp != pSrcEnd; pp += nStride)
	{
		*q = (*p - *(p - nStride)) & CSymbolBits<B>::maskval;
		if (DoCount)
			pCountTable[0][*q]++;
		p++;
		q++;

		for (; p < pp + nStride; p++, q++)
		{
			*q = (*p - (*(p - nStride) + *(p - 1) - *(p - 1 - nStride))) & CSymbolBits<B>::maskval;
			if (DoCount)
				pCountTable[0][*q]++;
		}
	}
}

template<int B>
void cpp_PredictPlanarGradientAndCount(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd, size_t cbStride, uint32_t pCountTable[][1 << B])
{
	cpp_PredictPlanarGradientAndMayCount<B, true>(pDst, pSrcBegin, pSrcEnd, cbStride, pCountTable);
}

template<int B>
void cpp_PredictPlanarGradient(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd, size_t cbStride)
{
	cpp_PredictPlanarGradientAndMayCount<B, false>(pDst, pSrcBegin, pSrcEnd, cbStride, NULL);
}

template void cpp_PredictPlanarGradientAndCount<8>(symbol_t<8> *pDst, const symbol_t<8> *pSrcBegin, const symbol_t<8> *pSrcEnd, size_t cbStride, uint32_t pCountTable[][256]);
template void cpp_PredictPlanarGradient<8>(symbol_t<8> *pDst, const symbol_t<8> *pSrcBegin, const symbol_t<8> *pSrcEnd, size_t cbStride);
template void cpp_PredictPlanarGradientAndCount<10>(symbol_t<10>* pDst, const symbol_t<10>* pSrcBegin, const symbol_t<10>* pSrcEnd, size_t cbStride, uint32_t pCountTable[][1024]);


template<int B>
void cpp_RestorePlanarGradient(symbol_t<B> *pDst, const symbol_t<B> *pSrcBegin, const symbol_t<B> *pSrcEnd, size_t cbStride)
{
	const symbol_t<B>* p = pSrcBegin;
	symbol_t<B>* q = pDst;
	size_t nStride = cbStride / sizeof(symbol_t<B>);

	*q = (*p + CSymbolBits<B>::midval) & CSymbolBits<B>::maskval;
	p++;
	q++;

	for (; p < pSrcBegin + nStride; p++, q++)
	{
		*q = (*p + *(q - 1)) & CSymbolBits<B>::maskval;
	}

	for (auto pp = pSrcBegin + nStride; pp != pSrcEnd; pp += nStride)
	{
		*q = (*p + *(q - nStride)) & CSymbolBits<B>::maskval;
		p++;
		q++;

		for (; p < pp + nStride; p++, q++)
		{
			*q = (*p + (*(q - nStride) + *(q - 1) - *(q - 1 - nStride))) & CSymbolBits<B>::maskval;
		}
	}
}

template void cpp_RestorePlanarGradient<8>(symbol_t<8> *pDst, const symbol_t<8> *pSrcBegin, const symbol_t<8> *pSrcEnd, size_t cbStride);
template void cpp_RestorePlanarGradient<10>(symbol_t<10>* pDst, const symbol_t<10>* pSrcBegin, const symbol_t<10>* pSrcEnd, size_t cbStride);
