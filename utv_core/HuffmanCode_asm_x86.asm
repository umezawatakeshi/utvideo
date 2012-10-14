; •¶ŽšƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e
; $Id$


%include "Common_asm_x86.mac"


section .text


%push

global _x86_i686_HuffmanEncode
_x86_i686_HuffmanEncode:
	SIMPLE_PROLOGUE	0, pDstBegin, pSrcBegin, pSrcEnd, pEncodeTable

	mov			esi, dword [esp + %$pSrcBegin]
	mov			edi, dword [esp + %$pDstBegin]
	mov			edx, dword [esp + %$pSrcEnd]
	mov			ebp, dword [esp + %$pEncodeTable]
	cmp			dword [ebp], 0
	je			.label3

	mov			bl, -32
	mov			cl, 0

	align		64
.label1:
	shld		eax, ecx, cl
	cmp			esi, edx
	jnb			.label4
	movzx		ecx, byte [esi]
	inc			esi
	mov			ecx, dword [ebp+ecx*4]
	add			bl, cl
	jnc			.label1
	sub			cl, bl
	shld		eax, ecx, cl
	mov			dword [edi], eax
	add			edi, 4
	add			cl, bl
	sub			bl, 32
	jmp			.label1

.label4:
	test		bl, 1fh
	jz			.label3
	neg			bl
	mov			cl, bl
	shl			eax, cl
	mov			dword [edi], eax
	add			edi, 4
.label3:
	mov			eax, edi
	sub			eax, dword [esp + %$pDstBegin]

	SIMPLE_EPILOGUE

%pop


%macro HUFFMAN_DECODE 7
%push
	MULTI_CONTEXT_XDEFINE procname, %1, accum, %2, step, %3, multiscan, %4, bottomup, %5, corrpos, %6, dummyalphapos, %7

global %$procname
%$procname:
	SIMPLE_PROLOGUE 12, pDstBegin, pDstEnd, pSrcBegin, pDecodeTable, cbNetWidth, cbGrossWidth

%define %$byCorrBuf     0
%define %$pLineEnd      4
%define %$dwLineOffset  8

	mov			esi, dword [esp + %$pSrcBegin]
	mov			ebx, dword [esp + %$pDecodeTable]
%if %$multiscan
 %if %$bottomup
	mov			edi, dword [esp + %$pDstEnd]
	sub			edi, dword [esp + %$cbGrossWidth]
	mov			eax, edi
	add			eax, dword [esp + %$cbNetWidth]
	mov			dword [esp + %$pLineEnd], eax
	mov			eax, dword [esp + %$cbGrossWidth]
	add			eax, dword [esp + %$cbNetWidth]
	mov			dword [esp + %$dwLineOffset], eax
 %else
	mov			edi, dword [esp + %$pDstBegin]
	mov			eax, edi
	add			eax, dword [esp + %$cbNetWidth]
	mov			dword [esp + %$pLineEnd], eax
	mov			eax, dword [esp + %$cbGrossWidth]
	sub			eax, dword [esp + %$cbNetWidth]
	mov			dword [esp + %$dwLineOffset], eax
 %endif
%else
	mov			edi, dword [esp + %$pDstBegin]
	mov			eax, dword [esp + %$pDstEnd]
	mov			dword [esp + %$pLineEnd], eax
%endif

%macro DO_OUTPUT_%$procname 1
%push
	MULTI_CONTEXT_XDEFINE dohuffman, %1

%if %$$accum
 %if %$$corrpos != 0
	mov			byte [esp + %$$byCorrBuf], 00h
 %else
	mov			byte [esp + %$$byCorrBuf], 80h
 %endif
%endif

%if %$dohuffman
	mov			cl, -32
	mov			ah, 32
	mov			edx, dword [esi]
	sub			esi, 4
%else
	mov			ah, byte [ebx]
%endif

	align		64
%%label1:
	cmp			edi, dword [esp + %$$pLineEnd]
	jae			%%label2

%if %$dohuffman
	add			cl, ah
	jnc			%%label4
	sub			cl, 32
	mov			ebp, edx
	mov			edx, dword [esi+4+4]
	add			esi, 4

%%label4:
	mov			eax, ebp
	shld		eax, edx, cl
	shr			eax, 20
	movzx		eax, word [ebx + eax*2]
	cmp			ah, 255
	jne			%%label0

	mov			eax, ebp
	shld		eax, edx, cl
	mov			ch, cl
	or			eax, 1
	bsr			ebp, eax
	mov			cl, byte [ebx + 8192 + ebp]					; pDecodeTable->nCodeShift[ebp]
	shr			eax, cl
	mov			cl, ch
	add			eax, dword [ebx + 8192+32 + ebp*4]			; pDecodeTable->dwSymbolBase[ebp]
	mov			eax, dword [ebx + 8192+32+4*32 + eax*2]		; pDecodeTable->SymbolAndCodeLength[eax]
	mov			ebp, dword [esi]

%%label0:
%else
	mov			al, ah
%endif

%if %$$accum
	add			al, byte [esp + %$$byCorrBuf]
	mov			byte [esp + %$$byCorrBuf], al
%endif
%if %$$corrpos != 0
	add			al, byte [edi + %$$corrpos]
%endif
	mov			byte [edi], al
%if %$$dummyalphapos != 0
	mov			byte [edi + %$$dummyalphapos], 0ffh
%endif
%if %$$step == 1
	inc			edi
%else
	add			edi, %$$step
%endif
	jmp			%%label1

%%label2:
%if %$$multiscan
 %if %$$bottomup
	mov			edx, dword [esp + %$$pLineEnd]
	sub			edx, dword [esp + %$$cbGrossWidth]
	cmp			edx, dword [esp + %$$pDstBegin]
	jbe			%%label3
	mov			dword [esp + %$$pLineEnd], edx
  %if %$dohuffman
	mov			edx, dword [esi+4]
  %endif
	sub			edi, dword [esp + %$$dwLineOffset]
 %else
	mov			edx, dword [esp + %$$pLineEnd]
	add			edx, dword [esp + %$$cbGrossWidth]
	cmp			edx, dword [esp + %$$pDstEnd]
	ja			%%label3
	mov			dword [esp + %$$pLineEnd], edx
  %if %$dohuffman
	mov			edx, dword [esi+4]
  %endif
	add			edi, dword [esp + %$$dwLineOffset]
 %endif
	jmp			%%label1
%%label3:
%endif
%pop
%endmacro

	cmp			byte [ebx + 1], 0
	je			.solidframe
	DO_OUTPUT_%$procname	1
	jmp			.funcend
.solidframe:
	DO_OUTPUT_%$procname	0
.funcend:
	SIMPLE_EPILOGUE

%pop
%endmacro

HUFFMAN_DECODE	_x86_i686_HuffmanDecode,                                             0, 1, 0, 0,  0,  0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccum,                                     1, 1, 0, 0,  0,  0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep2,                                1, 2, 0, 0,  0,  0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep4,                                1, 4, 0, 0,  0,  0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXGreen,            1, 4, 1, 1,  0,  0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXBlue,             1, 4, 1, 1, +1,  0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXRed,              1, 4, 1, 1, -1,  0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep4ForBottomupBGRXRedAndDummyAlpha, 1, 4, 1, 1, -1, +1
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRGreen,             1, 3, 1, 1,  0,  0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRBlue,              1, 3, 1, 1, +1,  0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep3ForBottomupBGRRed,               1, 3, 1, 1, -1,  0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBGreen,             1, 4, 1, 0,  0,  0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBBlue,              1, 4, 1, 0, -1,  0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBRed,               1, 4, 1, 0, +1,  0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep4ForTopdownXRGBRedAndDummyAlpha,  1, 4, 1, 0, +1, -1
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBGreen,              1, 3, 1, 0,  0,  0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBBlue,               1, 3, 1, 0, -1,  0
HUFFMAN_DECODE	_x86_i686_HuffmanDecodeAndAccumStep3ForTopdownRGBRed,                1, 3, 1, 0, +1,  0
