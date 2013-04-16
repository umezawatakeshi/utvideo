; 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ
; $Id$


%include "Common_asm_x86.mac"


section .text


	align	64
;			 fedcba9876543210
yoff	dq	00010001000100010h
		dq	00000000000000000h
uvoff	dq	00080008000800080h
		dq	00000000000000000h

y2rgb	dq	000004A8500004A85h
		dq	000004A8500004A85h
uv2g	dq	0E5FCF377E5FCF377h
		dq	0E5FCF377E5FCF377h
uv2r	dq	03313000033130000h
		dq	03313000033130000h
uv2b	dq	00000408D0000408Dh
		dq	00000408D0000408Dh

%macro CONVERT_ULY2_TO_RGB 4
%push
	MULTI_CONTEXT_XDEFINE procname, %1, bottomup, %2, littleendian, %3, rgb32, %4

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbGrossWidth, cbNetWidth

	mov			ebp, dword [esp + %$pYBegin]
	mov			ebx, dword [esp + %$pUBegin]
	mov			ecx, dword [esp + %$pVBegin]
	mov			edx, dword [esp + %$cbGrossWidth]
%if %$bottomup
	mov			esi, dword [esp + %$pDstEnd]			; esi なのに dst のポインタを保持するのは気持ちが悪いが。
	sub			esi, edx
	add			esi, dword [esp + %$cbNetWidth]
%else
	mov			esi, dword [esp + %$pDstBegin]			; esi なのに dst のポインタを保持するのは気持ちが悪いが。
	add			esi, dword [esp + %$cbNetWidth]
%endif

	pxor		xmm7, xmm7				; xmm7 = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	pcmpeqb		xmm6, xmm6				; xmm6 = ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
	psrlw		xmm6, 8					; xmm6 = 00 ff 00 ff 00 ff 00 ff 00 ff 00 ff 00 ff 00 ff

	align	64
.label0:
	mov			edi, esi
	sub			edi, dword [esp + %$cbNetWidth]

	; align	64	; さすがに入れすぎな気がするのでコメントアウト。
.label1:
	movd		xmm0, dword [ebp]		; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 Y3 Y2 Y1 Y0
	punpcklbw	xmm0, xmm7				; xmm0 = 00 00 00 00 00 00 00 00 00 Y3 00 Y2 00 Y1 00 Y0
	psubw		xmm0, oword [yoff]		; xmm0 = 00 00 00 00 00 00 00 00 ---Y3 ---Y2 ---Y1 ---Y0 (de-offset)
	punpcklwd	xmm0, xmm7				; xmm0 = 00 00 ---Y3 00 00 ---Y2 00 00 ---Y1 00 00 ---Y0 (de-offset)

	movd		xmm1, dword [ebx]		; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 U6 U4 U2 U0
	movd		xmm2, dword [ecx]		; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 V6 V4 V2 V0
	punpcklbw	xmm1, xmm2				; xmm1 = 00 00 00 00 00 00 00 00 V6 U6 V4 U4 V2 U2 V0 U0
	punpcklbw	xmm1, xmm7				; xmm1 = 00 V6 00 U6 00 V4 00 U4 00 V2 00 U2 00 V0 00 U0
	psubw		xmm1, oword [uvoff]		; xmm1 = ---V6 ---U6 ---V4 ---U4 ---V2 ---U2 ---V0 ---U0 (de-offset)
	punpckldq	xmm1, xmm1				; xmm1 = ---V2 ---U2 ---V2 ---U2 ---V0 ---U0 ---V0 ---U0 (de-offset)
	paddw		xmm1, xmm1

	pmaddwd		xmm0, oword [y2rgb]

	movdqa		xmm3, xmm1
	pmaddwd		xmm3, oword [uv2r]
	paddd		xmm3, xmm0				; xmm3 = -R3-------- -R2-------- -R1-------- -R0--------
	psrad		xmm3, 14				; xmm3 = ---------R3 ---------R2 ---------R1 ---------R0

	movdqa		xmm2, xmm1
	pmaddwd		xmm2, oword [uv2b]
	paddd		xmm2, xmm0				; xmm2 = -B3-------- -B2-------- -B1-------- -B0--------
	psrad		xmm2, 14				; xmm2 = ---------B3 ---------B2 ---------B1 ---------B0

	pmaddwd		xmm1, oword [uv2g]
	paddd		xmm1, xmm0				; xmm1 = -G3-------- -G2-------- -G1-------- -G0--------
	psrad		xmm1, 14				; xmm1 = ---------G3 ---------G2 ---------G1 ---------G0

%if %$littleendian
	packssdw	xmm1, xmm3				; xmm1 = ---R3 ---R2 ---R1 ---R0 ---G3 ---G2 ---G1 ---G0
	packssdw	xmm2, xmm7				; xmm2 = 00 00 00 00 00 00 00 00 ---B3 ---B2 ---B1 ---B0
	pmaxsw		xmm1, xmm7				; 計算過程でマイナスになることがあるので、
	pmaxsw		xmm2, xmm7				; ここの pmaxsw xmmN, xmm7 は必要。
	punpcklwd	xmm2, xmm1				; xmm2 = ---G3 ---B3 ---G2 ---B2 ---G1 ---B1 ---G0 ---B0
	punpckhwd	xmm1, xmm6				; xmm1 = 00 ff ---R3 00 ff ---R2 00 ff ---R1 00 ff ---R0
	packuswb	xmm2, xmm2				; xmm2 = XX XX XX XX XX XX XX XX G3 B3 G2 B2 G1 B1 G0 B0
	packuswb	xmm1, xmm1				; xmm1 = XX XX XX XX XX XX XX XX ff R3 ff R2 ff R1 ff R0
	punpcklwd	xmm2, xmm1				; xmm2 = ff R3 G3 B3 ff R2 G2 B2 ff R1 G1 B1 ff R0 G0 B0
%else
	packssdw	xmm3, xmm2				; xmm3 = ---B3 ---B2 ---B1 ---B0 ---R3 ---R2 ---R1 ---R0
	movdqa		xmm2, xmm6
	packssdw	xmm1, xmm1				; xmm1 = ---G3 ---G2 ---G1 ---G0 ---G3 ---G2 ---G1 ---G0
	pmaxsw		xmm3, xmm7				; 計算過程でマイナスになることがあるので、
	pmaxsw		xmm1, xmm7				; ここの pmaxsw xmmN, xmm7 は必要。
	punpcklwd	xmm2, xmm3				; xmm2 = ---R3 00 ff ---R2 00 ff ---R1 00 ff ---R0 00 ff
	punpckhwd	xmm1, xmm3				; xmm1 = ---B3 ---G3 ---B2 ---G2 ---B1 ---G1 ---B0 ---G0
	packuswb	xmm2, xmm2				; xmm2 = XX XX XX XX XX XX XX XX R3 ff R2 ff R1 ff R0 ff
	packuswb	xmm1, xmm1				; xmm1 = XX XX XX XX XX XX XX XX B3 G3 B2 G2 B1 G1 B0 G0
	punpcklwd	xmm2, xmm1				; xmm2 = B3 G3 R3 ff B2 G2 R2 ff B1 G1 R1 ff B0 G0 R0 ff
 %if ! %$rgb32
	; めんどくさいので
	movdqa		xmm1, xmm2
	psrldq		xmm2, 1
	pslldq		xmm1, 15
	por			xmm2, xmm1
 %endif
%endif

%if %$rgb32
	add			edi, 16
	add			ebp, 4
	add			ebx, 2
	add			ecx, 2
	cmp			edi, esi
	ja			.label2
	movdqu		oword [edi-16], xmm2
	jne			.label1
	jmp			.label3

.label2:
	movq		qword [edi-16], xmm2
	sub			edi, 8
	sub			ebp, 2
	sub			ebx, 1
	sub			ecx, 1
%else
	movd		eax, xmm2
	psrldq		xmm2, 4
	mov			word [edi], ax
	shr			eax, 16
	mov			byte [edi+2], al
	movd		eax, xmm2
	psrldq		xmm2, 4
	mov			word [edi+3], ax
	shr			eax, 16
	mov			byte [edi+5], al
	add			edi, 6
	add			ebp, 2
	add			ebx, 1
	add			ecx, 1
	cmp			edi, esi
	jae			.label3

	movd		eax, xmm2
	psrldq		xmm2, 4
	mov			word [edi], ax
	shr			eax, 16
	mov			byte [edi+2], al
	movd		eax, xmm2
	mov			word [edi+3], ax
	shr			eax, 16
	mov			byte [edi+5], al
	add			edi, 6
	add			ebp, 2
	add			ebx, 1
	add			ecx, 1
	cmp			edi, esi
	jb			.label1
%endif

.label3:
%if %$bottomup
	sub			esi, edx
	cmp			esi, dword [esp + %$pDstBegin]
	ja			.label0
%else
	add			esi, edx
	cmp			esi, dword [esp + %$pDstEnd]
	jb			.label0
%endif

	SIMPLE_EPILOGUE

%pop
%endmacro

CONVERT_ULY2_TO_RGB	_x86_sse2_ConvertULY2ToBottomupBGR,  1, 1, 0
CONVERT_ULY2_TO_RGB	_x86_sse2_ConvertULY2ToBottomupBGRX, 1, 1, 1
CONVERT_ULY2_TO_RGB	_x86_sse2_ConvertULY2ToTopdownBGR,   0, 1, 0
CONVERT_ULY2_TO_RGB	_x86_sse2_ConvertULY2ToTopdownBGRX,  0, 1, 1
CONVERT_ULY2_TO_RGB	_x86_sse2_ConvertULY2ToTopdownRGB,   0, 0, 0
CONVERT_ULY2_TO_RGB	_x86_sse2_ConvertULY2ToTopdownXRGB,  0, 0, 1


; Y  =  0.29891 R + 0.58661 G + 0.11448 B
; Cb = -0.16874 R - 0.33126 G + 0.50000 B
; Cr =  0.50000 R - 0.41869 G - 0.08131 B

; Y  =  0.257 R + 0.504 G + 0.098 B + 16
; Cb = -0.148 R - 0.291 G + 0.439 B + 128
; Cr =  0.439 R - 0.368 G - 0.071 B + 128

	align	64
;			 fedcba9876543210
;b2yuv	dq	0064b00000000064bh
;		dq	0fdb7fdb70e0e0e0eh
;g2yuv	dq	0203e00000000203eh
;		dq	0f3fbf3fbf6b0f6b0h
;r2yuv	dq	0106e00000000106eh
;		dq	00e0e0e0efb42fb42h
b2yuv	dq	00646000000000646h
		dq	0fdbafdba0e0c0e0ch
g2yuv	dq	02042000000002042h
		dq	0f439f439f6b0f6b0h
r2yuv	dq	01073000000001073h
		dq	00e0c0e0cfb44fb44h
yuvoff	dq	00004200000042000h
		dq	00020200000202000h

%macro CONVERT_RGB_TO_ULY2 4
%push
	MULTI_CONTEXT_XDEFINE procname, %1, bottomup, %2, littleendian, %3, rgb32, %4

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbGrossWidth, cbNetWidth

	mov			edi, dword [esp + %$pYBegin]
	mov			ebx, dword [esp + %$pUBegin]
	mov			ecx, dword [esp + %$pVBegin]
%if %$bottomup
	mov			ebp, dword [esp + %$pSrcEnd]
	sub			ebp, dword [esp + %$cbGrossWidth]
	add			ebp, dword [esp + %$cbNetWidth]
%else
	mov			ebp, dword [esp + %$pSrcBegin]
	add			ebp, dword [esp + %$cbNetWidth]
%endif

	align	64
.label0:
	mov			esi, ebp
	sub			esi, dword [esp + %$cbNetWidth]
%if %$rgb32
	add			esi, 4
%else
	add			esi, 3
%endif

	;align	64
.label1:
%if %$rgb32
	movd		xmm0, dword [esi-4]					; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R0 G0 B0 / B0 G0 R0 XX
	movd		xmm1, dword [esi]					; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R1 G1 B1 / B1 G1 R1 XX
%else
	movd		xmm0, dword [esi-3]					; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R0 G0 B0 / XX B0 G0 R0
	movd		xmm1, dword [esi-1]					; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 R1 G1 B1 XX / B1 G1 R1 XX
	psrld		xmm1, 8								; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 00 R1 G1 B1 / 00 B1 G1 R1
%endif

	punpcklbw	xmm0, xmm1							; xmm0 = 00 00 00 00 00 00 00 00 00 XX R1 R0 G1 G0 B1 B0 / B1 B0 G1 G0 R1 R0 XX XX
													;                                                        / XX XX B1 B0 G1 G0 R1 R0 (rgb24be)
.label3:
	pxor		xmm1, xmm1
	punpcklbw	xmm0, xmm1							; xmm0 = 00 00 00 XX 00 R1 00 R0 00 G1 00 G0 00 B1 00 B0

%if %$littleendian
	pshufd		xmm1, xmm0, 055h					; xmm1 = 00 G1 00 G0 00 G1 00 G0 00 G1 00 G0 00 G1 00 G0
	pshufd		xmm2, xmm0, 0aah					; xmm2 = 00 R1 00 R0 00 R1 00 R0 00 R1 00 R0 00 R1 00 R0
	pshufd		xmm0, xmm0, 000h					; xmm0 = 00 B1 00 B0 00 B1 00 B0 00 B1 00 B0 00 B1 00 B0
%else
 %if %$rgb32
	pshufd		xmm1, xmm0, 0aah					; xmm1 = 00 G1 00 G0 00 G1 00 G0 00 G1 00 G0 00 G1 00 G0
	pshufd		xmm2, xmm0, 055h					; xmm2 = 00 R1 00 R0 00 R1 00 R0 00 R1 00 R0 00 R1 00 R0
	pshufd		xmm0, xmm0, 0ffh					; xmm0 = 00 B1 00 B0 00 B1 00 B0 00 B1 00 B0 00 B1 00 B0
 %else
	pshufd		xmm1, xmm0, 055h					; xmm1 = 00 G1 00 G0 00 G1 00 G0 00 G1 00 G0 00 G1 00 G0
	pshufd		xmm2, xmm0, 000h					; xmm2 = 00 R1 00 R0 00 R1 00 R0 00 R1 00 R0 00 R1 00 R0
	pshufd		xmm0, xmm0, 0aah					; xmm0 = 00 B1 00 B0 00 B1 00 B0 00 B1 00 B0 00 B1 00 B0
 %endif
%endif

	pmaddwd		xmm0, oword [b2yuv]					; xmm0 = ----B2V---- ----B2U---- ----B2Y1--- ----B2Y0---
	pmaddwd		xmm1, oword [g2yuv]					; xmm1 = ----G2V---- ----G2U---- ----G2Y1--- ----G2Y0---
	pmaddwd		xmm2, oword [r2yuv]					; xmm2 = ----R2V---- ----R2U---- ----R2Y1--- ----R2Y0---

	paddd		xmm0, oword [yuvoff]
	paddd		xmm2, xmm1
	paddd		xmm0, xmm2							; xmm0 = -----V----- -----U----- -----Y1---- -----Y0----

	psrld		xmm0, 14							; xmm0 = ---------V0 ---------U0 ---------Y1 ---------Y0
	packssdw	xmm0, xmm0							; xmm0 = XX XX XX XX XX XX XX XX ---V0 ---U0 ---Y1 ---Y0
	packuswb	xmm0, xmm0							; xmm0 = XX XX XX XX XX XX XX XX XX XX XX XX V0 U0 Y1 Y0
	movd		eax, xmm0
	mov			word [edi], ax
	shr			eax, 16
	mov			byte [ebx], al
	mov			byte [ecx], ah

	add			edi, 2
	add			ebx, 1
	add			ecx, 1
%if %$rgb32
	add			esi, 8
%else
	add			esi, 6
%endif
	cmp			esi, ebp
	jb			.label1
	ja			.label2

%if %$rgb32
	movd		xmm0, dword [esi-4]					; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R0 G0 B0
%else
	movd		xmm0, dword [esi-3]					; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R0 G0 B0
%endif

	punpcklbw	xmm0, xmm0							; xmm0 = 00 00 00 00 00 00 00 00 XX XX R0 R0 G0 G0 B0 B0
	jmp			.label3

.label2:
%if %$bottomup
	sub			ebp, dword [esp + %$cbGrossWidth]
	cmp			ebp, dword [esp + %$pSrcBegin]
	ja			.label0
%else
	add			ebp, dword [esp + %$cbGrossWidth]
	cmp			ebp, dword [esp + %$pSrcEnd]
	jbe			.label0
%endif

	SIMPLE_EPILOGUE

%pop
%endmacro

CONVERT_RGB_TO_ULY2	_x86_sse2_ConvertBottomupBGRToULY2,  1, 1, 0
CONVERT_RGB_TO_ULY2	_x86_sse2_ConvertBottomupBGRXToULY2, 1, 1, 1
CONVERT_RGB_TO_ULY2	_x86_sse2_ConvertTopdownBGRToULY2,   0, 1, 0
CONVERT_RGB_TO_ULY2	_x86_sse2_ConvertTopdownBGRXToULY2,  0, 1, 1
CONVERT_RGB_TO_ULY2	_x86_sse2_ConvertTopdownRGBToULY2,   0, 0, 0
CONVERT_RGB_TO_ULY2	_x86_sse2_ConvertTopdownXRGBToULY2,  0, 0, 1




	align	64
bgr2planarpshufb16	dq	05020f0c09060300h
					dq	0d0a0704010e0b08h
bgrx2planarpshufb16	dq	0d0905010c080400h
					dq	0f0b07030e0a0602h
xrgb2planarpshufb16	dq	0e0a06020f0b0703h
					dq	0c0804000d090501h
_0x80b				dq	8080808080808080h
					dq	8080808080808080h

%push
global _x86_avx1_ConvertBGRToULRG
_x86_avx1_ConvertBGRToULRG:
	SIMPLE_PROLOGUE	0, pGBegin, pBBegin, pRBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride

	vmovdqa		xmm7, [bgr2planarpshufb16]
	vmovdqa		xmm6, [_0x80b]
	mov			esi, [esp + %$pSrcBegin]
	mov			edi, [esp + %$pGBegin]
	mov			ebx, [esp + %$pBBegin]
	mov			edx, [esp + %$pRBegin]

.label1:
	mov			ecx, esi
	add			ecx, [esp + %$cbWidth]
	sub			ecx, 48 - 3
.label0:
	vlddqu		xmm0, [esi   ]			; xmm0 = B5 R4 G4 B4 R3 G3 B3 R2 G2 B2 R1 G1 B1 R0 G0 B0
	vlddqu		xmm1, [esi+16]			; xmm1 = Ga Ba R9 G9 B9 R8 G8 B8 R7 G7 B7 R6 G6 B6 R5 G5
	vlddqu		xmm2, [esi+32]			; xmm2 = Rf Gf Bf Re Ge Be Rd Gd Bd Rc Gc Bc Rb Gb Bb Ra

	vpshufb		xmm0, xmm0, xmm7		; xmm0 = G4 G3 G2 G1 G0 R4 R3 R2 R1 R0 B5 B4 B3 B2 B1 B0
	vpshufb		xmm1, xmm1, xmm7		; xmm1 = R9 R8 R7 R6 R5 Ba B9 B8 B7 B6 Ga G9 G8 G7 G6 G5
	vpshufb		xmm2, xmm2, xmm7		; xmm2 = Bf Be Bd Bc Bb Gf Ge Gd Gc Gb Rf Re Rd Rc Rb Ra

	vpalignr	xmm5, xmm2, xmm1, 6		; xmm5 = Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 Ba B9 B8 B7 B6
	vpalignr	xmm4, xmm1, xmm0, 6		; xmm4 = Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0 R4 R3 R2 R1 R0
	vpalignr	xmm3, xmm0, xmm2, 6		; xmm3 = B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb Gf Ge Gd Gc Gb

	vpalignr	xmm0, xmm3, xmm4, 5		; xmm0 = Gf Ge Gd Gc Gb Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
	vpalignr	xmm1, xmm4, xmm5, 5		; xmm1 = R4 R3 R2 R1 R0 Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5
	vpalignr	xmm2, xmm5, xmm3, 5		; xmm2 = Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb

	vmovdqu		[edi], xmm0
	vpsubb		xmm0, xmm0, xmm6

	vpalignr	xmm1, xmm1, xmm1, 11	; xmm1 = Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 R4 R3 R2 R1 R0
	vpalignr	xmm2, xmm2, xmm2, 5		; xmm2 = Bf Be Bd Bc Bb Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0

	vpsubb		xmm1, xmm1, xmm0
	vpsubb		xmm2, xmm2, xmm0
	vmovdqu		[ebx], xmm2
	vmovdqu		[edx], xmm1

	add			edi, 16
	add			ebx, 16
	add			edx, 16
	add			esi, 48
	cmp			esi, ecx
	jb			.label0

	add			ecx, 48 - 3

.label3:
	cmp			esi, ecx
	jae			.label2
	mov			al, [esi+1]
	mov			[edi], al
	sub			al, 80h

	mov			ah, [esi  ]
	sub			ah, al
	mov			[ebx], ah

	mov			ah, [esi+2]
	sub			ah, al
	mov			[edx], ah

	add			edi, 1
	add			ebx, 1
	add			edx, 1
	add			esi, 3
	jmp			.label3

.label2:
	sub			esi, [esp + %$cbWidth]
	add			esi, [esp + %$scbStride]
	cmp			esi, [esp + %$pSrcEnd]
	jne			.label1

	SIMPLE_EPILOGUE
%pop


;dummy0			dq	3121110130201000h
;				dq	3323130332221202h
;dummy1			dq	3525150534241404h
;				dq	3727170736261606h
;dummy2			dq	3929190938281808h
;				dq	3b2b1b0b3a2a1a0ah
;dummy3			dq	3d2d1d0d3c2c1c0ch
;				dq	3f2f1f0f3e2e1e0eh

%macro CONVERT_XRGB_TO_ULRGA 3
%push
	MULTI_CONTEXT_XDEFINE procname, %1, bgrx, %2, havealpha, %3

global %$procname
%$procname:
%if %$havealpha
	SIMPLE_PROLOGUE 0, pGBegin, pBBegin, pRBegin, pABegin, pSrcBegin, pSrcEnd, cbWidth, scbStride
%else
	SIMPLE_PROLOGUE 0, pGBegin, pBBegin, pRBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride
%endif

%if %$bgrx
	vmovdqa		xmm7, [bgrx2planarpshufb16]
%else
	vmovdqa		xmm7, [xrgb2planarpshufb16]
%endif
	vmovdqa		xmm6, [_0x80b]
	mov			esi, [esp + %$pSrcBegin]
	mov			edi, [esp + %$pGBegin]
	mov			ebx, [esp + %$pBBegin]
	mov			edx, [esp + %$pRBegin]
%if %$havealpha
	mov			ebp, [esp + %$pABegin]
%endif

.label1:
	mov			ecx, esi
	add			ecx, [esp + %$cbWidth]
	sub			ecx, 64 - 4
.label0:
	vlddqu		xmm0, [esi   ]		; xmm0 = A3 R3 G3 B3 A2 R2 G2 B2 A1 R1 G1 B1 A0 R0 G0 B0 (bgrx)
	vlddqu		xmm1, [esi+16]		; xmm1 = A7 R7 G7 B7 A6 R6 G6 B6 A5 R5 G5 B5 A4 R4 G4 B4
	vlddqu		xmm2, [esi+32]		; xmm2 = Ab Rb Gb Bb Aa Ra Ga Ba A9 R9 G9 B9 A8 R8 G8 B8
	vlddqu		xmm3, [esi+48]		; xmm3 = Af Rf Gf Bf Ae Re Ge Be Ad Rd Gd Bd Ac Rc Gc Bc

	vpshufb		xmm0, xmm0, xmm7	; xmm0 = A3 A2 A1 A0 R3 R2 R1 R0 G3 G2 G1 G0 B3 B2 B1 B0
	vpshufb		xmm1, xmm1, xmm7	; xmm1 = A7 A6 A5 A4 R7 R6 R5 R4 G7 G6 G5 G4 B7 B6 B5 B4
	vpshufb		xmm2, xmm2, xmm7	; xmm2 = Ab Aa A9 A8 Rb Ra R9 R8 Gb Ga G9 G8 Bb Ba B9 B8
	vpshufb		xmm3, xmm3, xmm7	; xmm3 = Af Ae Ad Ac Rf Re Rd Rc Gf Ge Gd Gc Bf Be Bd Bc

	vpunpckldq	xmm4, xmm0, xmm1	; xmm4 = G7 G6 G5 G4 G3 G2 G1 G0 B7 B6 B5 B4 B3 B2 B1 B0
	vpunpckhdq	xmm0, xmm0, xmm1	; xmm0 = A7 A6 A5 A4 A3 A2 A1 A0 R7 R6 R5 R4 R3 R2 R1 R0
	vpunpckldq	xmm5, xmm2, xmm3	; xmm6 = Gf Ge Gd Gc Gb Ga G9 G8 Bf Be Bd Bc Bb Ba B9 B8
	vpunpckhdq	xmm2, xmm2, xmm3	; xmm2 = Af Ae Ad Ac Ab Aa A9 A8 Rf Re Rd Rc Rb Ra R9 R8

	vpunpcklqdq	xmm1, xmm4, xmm5	; xmm1 = Bf Be Bd Bc Bb Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0
	vpunpckhqdq	xmm4, xmm4, xmm5	; xmm4 = Gf Ge Gd Gc Gb Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
	vpunpcklqdq	xmm3, xmm0, xmm2	; xmm3 = Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 R4 R3 R2 R1 R0
%if %$havealpha
	vpunpckhqdq	xmm0, xmm0, xmm2	; xmm0 = Af Ae Ad Ac Ab Aa A9 A8 A7 A6 A5 A4 A3 A2 A1 A0
%endif

	vmovdqu		[edi], xmm4
	vpsubb		xmm4, xmm4, xmm6
%if %$havealpha
	vmovdqu		[ebp], xmm0
%endif

	vpsubb		xmm1, xmm1, xmm4
	vmovdqu		[ebx], xmm1
	vpsubb		xmm3, xmm3, xmm4
	vmovdqu		[edx], xmm3

	add			edi, 16
	add			ebx, 16
	add			edx, 16
%if %$havealpha
	add			ebp, 16
%endif
	add			esi, 64
	cmp			esi, ecx
	jb			.label0

	add			ecx, 64 - 4

.label3:
	cmp			esi, ecx
	jae			.label2
%if %$bgrx
	mov			al, [esi+1]
%else
	mov			al, [esi+2]
%endif
	mov			[edi], al
	sub			al, 80h

%if %$havealpha
 %if %$bgrx
	mov			ah, [esi+3]
 %else
	mov			ah, [esi  ]
 %endif
	mov			[ebp], ah
%endif

%if %$bgrx
	mov			ah, [esi  ]
%else
	mov			al, [esi+3]
%endif
	sub			ah, al
	mov			[ebx], ah

%if %$bgrx
	mov			ah, [esi+2]
%else
	mov			al, [esi+1]
%endif
	sub			ah, al
	mov			[edx], ah

	add			edi, 1
	add			ebx, 1
	add			edx, 1
%if %$havealpha
	add			ebp, 1
%endif
	add			esi, 4
	jmp			.label3

.label2:
	sub			esi, [esp + %$cbWidth]
	add			esi, [esp + %$scbStride]
	cmp			esi, [esp + %$pSrcEnd]
	jne			.label1

	SIMPLE_EPILOGUE
%pop
%endmacro

CONVERT_XRGB_TO_ULRGA	_x86_avx1_ConvertBGRXToULRG, 1, 0
CONVERT_XRGB_TO_ULRGA	_x86_avx1_ConvertXRGBToULRG, 0, 0
CONVERT_XRGB_TO_ULRGA	_x86_avx1_ConvertBGRAToULRA, 1, 1
CONVERT_XRGB_TO_ULRGA	_x86_avx1_ConvertARGBToULRA, 0, 1
