/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */
/*
 * Ut Video Codec Suite
 * Copyright (C) 2008  UMEZAWA Takeshi
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 * 
 * Ut Video Codec Suite
 * Copyright (C) 2008  �~�V �Ўu
 * 
 * ���̃v���O�����̓t���[�\�t�g�E�F�A�ł��B���Ȃ��͂�����A�t���[�\�t
 * �g�E�F�A���c�ɂ���Ĕ��s���ꂽ GNU ��ʌ��O���p�����_��(�o�[�W��
 * ��2���A��]�ɂ���Ă͂���ȍ~�̃o�[�W�����̂����ǂꂩ)�̒�߂����
 * �̉��ōĔЕz�܂��͉��ς��邱�Ƃ��ł��܂��B
 * 
 * ���̃v���O�����͗L�p�ł��邱�Ƃ�����ĔЕz����܂����A*�S���̖���
 * ��* �ł��B���Ɖ\���̕ۏ؂����̖ړI�ւ̓K�����́A���O�Ɏ����ꂽ
 * ���̂��܂ߑS�����݂��܂���B�ڂ�����GNU ��ʌ��O���p�����_�񏑂���
 * �����������B
 * 
 * ���Ȃ��͂��̃v���O�����Ƌ��ɁAGNU ��ʌ��O���p�����_�񏑂̕�������
 * �ꕔ�󂯎�����͂��ł��B�����󂯎���Ă��Ȃ���΁A�t���[�\�t�g�E�F
 * �A���c�܂Ő������Ă�������(����� the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA)�B
 */

#include "StdAfx.h"
#include "Predict.h"

inline BYTE median(BYTE a, BYTE b, BYTE c)
{
	return max(min(max(a,b),c),min(a,b));
}

void PredictMedian(BYTE *pDst, const BYTE *pSrcStart, const BYTE *pSrcEnd, DWORD dwStride)
{
	const BYTE *p = pSrcStart;
	BYTE *q = pDst;

	for (; p < pSrcEnd; p++, q++)
	{
		// *q = *p - *(p - dwStride) - *(p - 1) + *(p - 1 - dwStride); // gradient
		*q = *p - median(*(p - dwStride), *(p - 1), *(p - dwStride)+ *(p - 1) - *(p - 1 - dwStride));
	}
}

void RestoreMedian(BYTE *pDst, const BYTE *pSrcStart, const BYTE *pSrcEnd, DWORD dwStride)
{
	const BYTE *p = pSrcStart;
	BYTE *q = pDst;

	for (; p < pSrcEnd; p++, q++)
	{
		*q = *p + median(*(q - dwStride), *(q - 1), *(q - dwStride)+ *(q - 1) - *(q - 1 - dwStride));
	}
}