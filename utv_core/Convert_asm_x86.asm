; 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ
; $Id$


%include "Common_asm_x86.mac"


section .text


%include "Coefficient_asm_x86x64.mac"


%macro CONVERT_ULY2_TO_RGB 4
%push
	MULTI_CONTEXT_XDEFINE procname, %1, coeffvar, %2, littleendian, %3, rgb32, %4

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride

	mov			edi, [esp + %$pDstBegin]
	mov			esi, [esp + %$pYBegin]
	mov			ebx, [esp + %$pUBegin]
	mov			edx, [esp + %$pVBegin]

	pxor		xmm7, xmm7				; xmm7 = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	pcmpeqb		xmm6, xmm6				; xmm6 = ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff
	psrlw		xmm6, 8					; xmm6 = 00 ff 00 ff 00 ff 00 ff 00 ff 00 ff 00 ff 00 ff

	align	64
.label0:
	mov			ecx, edi
	add			ecx, [esp + %$cbWidth]

	; align	64	; さすがに入れすぎな気がするのでコメントアウト。
.label1:
	movd		xmm0, [esi]				; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 Y3 Y2 Y1 Y0
	punpcklbw	xmm0, xmm7				; xmm0 = 00 00 00 00 00 00 00 00 00 Y3 00 Y2 00 Y1 00 Y0
	psubw		xmm0, [yoff]			; xmm0 = 00 00 00 00 00 00 00 00 ---Y3 ---Y2 ---Y1 ---Y0 (de-offset)
	punpcklwd	xmm0, xmm7				; xmm0 = 00 00 ---Y3 00 00 ---Y2 00 00 ---Y1 00 00 ---Y0 (de-offset)

	movd		xmm1, [ebx]				; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 U6 U4 U2 U0
	movd		xmm2, [edx]				; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 V6 V4 V2 V0
	punpcklbw	xmm1, xmm2				; xmm1 = 00 00 00 00 00 00 00 00 V6 U6 V4 U4 V2 U2 V0 U0
	punpcklbw	xmm1, xmm7				; xmm1 = 00 V6 00 U6 00 V4 00 U4 00 V2 00 U2 00 V0 00 U0
	psubw		xmm1, [uvoff]			; xmm1 = ---V6 ---U6 ---V4 ---U4 ---V2 ---U2 ---V0 ---U0 (de-offset)
	punpckldq	xmm1, xmm1				; xmm1 = ---V2 ---U2 ---V2 ---U2 ---V0 ---U0 ---V0 ---U0 (de-offset)

	pmaddwd		xmm0, [%$coeffvar + yuvcoeff.y2rgb]

	movdqa		xmm3, xmm1
	pmaddwd		xmm3, [%$coeffvar + yuvcoeff.uv2r]
	paddd		xmm3, xmm0				; xmm3 = -R3-------- -R2-------- -R1-------- -R0--------
	psrad		xmm3, 13				; xmm3 = ---------R3 ---------R2 ---------R1 ---------R0

	movdqa		xmm2, xmm1
	pmaddwd		xmm2, [%$coeffvar + yuvcoeff.uv2b]
	paddd		xmm2, xmm0				; xmm2 = -B3-------- -B2-------- -B1-------- -B0--------
	psrad		xmm2, 13				; xmm2 = ---------B3 ---------B2 ---------B1 ---------B0

	pmaddwd		xmm1, [%$coeffvar + yuvcoeff.uv2g]
	paddd		xmm1, xmm0				; xmm1 = -G3-------- -G2-------- -G1-------- -G0--------
	psrad		xmm1, 13				; xmm1 = ---------G3 ---------G2 ---------G1 ---------G0

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
	add			esi, 4
	add			ebx, 2
	add			edx, 2
	cmp			edi, ecx
	ja			.label2
	movdqu		[edi-16], xmm2
	jne			.label1
	jmp			.label3

.label2:
	movq		[edi-16], xmm2
	sub			edi, 8
	sub			esi, 2
	sub			ebx, 1
	sub			edx, 1
%else
	movd		eax, xmm2
	psrldq		xmm2, 4
	mov			[edi], ax
	shr			eax, 16
	mov			[edi+2], al
	movd		eax, xmm2
	psrldq		xmm2, 4
	mov			[edi+3], ax
	shr			eax, 16
	mov			[edi+5], al
	add			edi, 6
	add			esi, 2
	add			ebx, 1
	add			edx, 1
	cmp			edi, ecx
	jae			.label3

	movd		eax, xmm2
	psrldq		xmm2, 4
	mov			[edi], ax
	shr			eax, 16
	mov			[edi+2], al
	movd		eax, xmm2
	mov			[edi+3], ax
	shr			eax, 16
	mov			[edi+5], al
	add			edi, 6
	add			esi, 2
	add			ebx, 1
	add			edx, 1
	cmp			edi, ecx
	jb			.label1
%endif

.label3:
	sub			edi, [esp + %$cbWidth]
	add			edi, [esp + %$scbStride]
	cmp			edi, [esp + %$pDstEnd]
	jne			.label0

	SIMPLE_EPILOGUE

%pop
%endmacro

CONVERT_ULY2_TO_RGB	_sse2_ConvertULY2ToBGR,   bt601coeff, 1, 0
CONVERT_ULY2_TO_RGB	_sse2_ConvertULY2ToBGRX,  bt601coeff, 1, 1
CONVERT_ULY2_TO_RGB	_sse2_ConvertULY2ToRGB,   bt601coeff, 0, 0
CONVERT_ULY2_TO_RGB	_sse2_ConvertULY2ToXRGB,  bt601coeff, 0, 1
CONVERT_ULY2_TO_RGB	_sse2_ConvertULH2ToBGR,   bt709coeff, 1, 0
CONVERT_ULY2_TO_RGB	_sse2_ConvertULH2ToBGRX,  bt709coeff, 1, 1
CONVERT_ULY2_TO_RGB	_sse2_ConvertULH2ToRGB,   bt709coeff, 0, 0
CONVERT_ULY2_TO_RGB	_sse2_ConvertULH2ToXRGB,  bt709coeff, 0, 1


align	64
shufdata32	dq	8005800180048000h
			dq	8007800380068002h

%macro CONVERT_RGB_TO_ULY2 5
%push
	MULTI_CONTEXT_XDEFINE procname, %1, use_ssse3, %2, coeffvar, %3, littleendian, %4, rgb32, %5

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride

	mov			esi, [esp + %$pSrcBegin]
	mov			edi, [esp + %$pYBegin]
	mov			ebx, [esp + %$pUBegin]
	mov			edx, [esp + %$pVBegin]

%if %$use_ssse3
 %if %$rgb32
	movdqa		xmm3, [shufdata32]
 %else
	NOTYET
 %endif
%endif
	movdqa		xmm4, [%$coeffvar + yuvcoeff.b2yuv]
	movdqa		xmm5, [%$coeffvar + yuvcoeff.g2yuv]
	movdqa		xmm6, [%$coeffvar + yuvcoeff.r2yuv]
	movdqa		xmm7, [yuvoff]

	align	64
.label0:
	mov			ecx, esi
	add			ecx, [esp + %$cbWidth]

	;align	64
.label1:
%if ! %$use_ssse3
 %if %$rgb32
	movd		xmm0, [esi  ]						; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R0 G0 B0 / B0 G0 R0 XX
	movd		xmm1, [esi+4]						; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R1 G1 B1 / B1 G1 R1 XX
 %else
	movd		xmm0, [esi  ]						; xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 XX R0 G0 B0 / XX B0 G0 R0
	movd		xmm1, [esi+2]						; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 R1 G1 B1 XX / B1 G1 R1 XX
	psrld		xmm1, 8								; xmm1 = 00 00 00 00 00 00 00 00 00 00 00 00 00 R1 G1 B1 / 00 B1 G1 R1
 %endif

	punpcklbw	xmm0, xmm1							; xmm0 = 00 00 00 00 00 00 00 00 00 XX R1 R0 G1 G0 B1 B0 / B1 B0 G1 G0 R1 R0 XX XX
													;                                                        / XX XX B1 B0 G1 G0 R1 R0 (rgb24be)
	pxor		xmm1, xmm1
	punpcklbw	xmm0, xmm1							; xmm0 = 00 00 00 XX 00 R1 00 R0 00 G1 00 G0 00 B1 00 B0
													;        00 B1 00 B0 00 G1 00 G0 00 R1 00 R0 00 XX 00 XX (rgb32be)
													;        00 00 00 XX 00 B1 00 B0 00 G1 00 G0 00 R1 00 R0 (rgb24be)
%else
 %if %$rgb32
	movq		xmm0, [esi]							; xmm0 = 00 00 00 00 00 00 00 00 XX R1 G1 B1 XX R0 G0 B0 / B1 G1 R1 XX B0 G0 R0 XX
	pshufb		xmm0, xmm3							; xmm0 = 00 XX 00 XX 00 R1 00 R0 00 G1 00 G0 00 B1 00 B0
													;        00 B1 00 B0 00 G1 00 G0 00 R1 00 R0 00 XX 00 XX (rgb32be)
 %else
	NOTYET
 %endif
%endif

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

	pmaddwd		xmm0, xmm4							; xmm0 = ----B2V---- ----B2U---- ----B2Y1--- ----B2Y0---
	pmaddwd		xmm1, xmm5							; xmm1 = ----G2V---- ----G2U---- ----G2Y1--- ----G2Y0---
	pmaddwd		xmm2, xmm6							; xmm2 = ----R2V---- ----R2U---- ----R2Y1--- ----R2Y0---

	paddd		xmm0, xmm7
	paddd		xmm2, xmm1
	paddd		xmm0, xmm2							; xmm0 = -----V----- -----U----- -----Y1---- -----Y0----

	psrld		xmm0, 14							; xmm0 = ---------V0 ---------U0 ---------Y1 ---------Y0
	packssdw	xmm0, xmm0							; xmm0 = XX XX XX XX XX XX XX XX ---V0 ---U0 ---Y1 ---Y0
	packuswb	xmm0, xmm0							; xmm0 = XX XX XX XX XX XX XX XX XX XX XX XX V0 U0 Y1 Y0
	movd		eax, xmm0
	mov			[edi], ax
	shr			eax, 16
	mov			[ebx], al
	mov			[edx], ah

	add			edi, 2
	add			ebx, 1
	add			edx, 1
%if %$rgb32
	add			esi, 8
%else
	add			esi, 6
%endif
	cmp			esi, ecx
	jb			.label1

.label2:
	sub			esi, [esp + %$cbWidth]
	add			esi, [esp + %$scbStride]
	cmp			esi, [esp + %$pSrcEnd]
	jne			.label0

	SIMPLE_EPILOGUE

%pop
%endmacro

CONVERT_RGB_TO_ULY2	_sse2_ConvertBGRToULY2,   0, bt601coeff, 1, 0
CONVERT_RGB_TO_ULY2	_sse2_ConvertBGRXToULY2,  0, bt601coeff, 1, 1
CONVERT_RGB_TO_ULY2	_sse2_ConvertRGBToULY2,   0, bt601coeff, 0, 0
CONVERT_RGB_TO_ULY2	_sse2_ConvertXRGBToULY2,  0, bt601coeff, 0, 1
CONVERT_RGB_TO_ULY2	_sse2_ConvertBGRToULH2,   0, bt709coeff, 1, 0
CONVERT_RGB_TO_ULY2	_sse2_ConvertBGRXToULH2,  0, bt709coeff, 1, 1
CONVERT_RGB_TO_ULY2	_sse2_ConvertRGBToULH2,   0, bt709coeff, 0, 0
CONVERT_RGB_TO_ULY2	_sse2_ConvertXRGBToULH2,  0, bt709coeff, 0, 1

;CONVERT_RGB_TO_ULY2	_ssse3_ConvertBGRToULY2,  1, bt601coeff, 1, 0
CONVERT_RGB_TO_ULY2	_ssse3_ConvertBGRXToULY2, 1, bt601coeff, 1, 1
;CONVERT_RGB_TO_ULY2	_ssse3_ConvertRGBToULY2,  1, bt601coeff, 0, 0
CONVERT_RGB_TO_ULY2	_ssse3_ConvertXRGBToULY2, 1, bt601coeff, 0, 1
;CONVERT_RGB_TO_ULY2	_ssse3_ConvertBGRToULH2,   1, bt709coeff, 1, 0
CONVERT_RGB_TO_ULY2	_ssse3_ConvertBGRXToULH2,  1, bt709coeff, 1, 1
;CONVERT_RGB_TO_ULY2	_ssse3_ConvertRGBToULH2,   1, bt709coeff, 0, 0
CONVERT_RGB_TO_ULY2	_ssse3_ConvertXRGBToULH2,  1, bt709coeff, 0, 1




	align	64
bgr2planarpshufb16	dq	05020f0c09060300h
					dq	0d0a0704010e0b08h
planar2bgrpshufb16	dq	0d02070c01060b00h
					dq	050a0f04090e0308h

bgrx2planarpshufb16	dq	0d0905010c080400h
					dq	0f0b07030e0a0602h
planar2bgrxpshufb16	dq	0d0905010c080400h	; 計算してみたら実は同じだという…
					dq	0f0b07030e0a0602h

xrgb2planarpshufb16	dq	0e0a06020f0b0703h
					dq	0c0804000d090501h
planar2xrgbpshufb16	dq	0105090d0004080ch
					dq	03070b0f02060a0eh

_0x80b				dq	8080808080808080h
					dq	8080808080808080h

%push
global _ssse3_ConvertBGRToULRG
_ssse3_ConvertBGRToULRG:
	SIMPLE_PROLOGUE	0, pGBegin, pBBegin, pRBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride

	movdqa		xmm7, [bgr2planarpshufb16]
	movdqa		xmm6, [_0x80b]
	mov			esi, [esp + %$pSrcBegin]
	mov			edi, [esp + %$pGBegin]
	mov			ebx, [esp + %$pBBegin]
	mov			edx, [esp + %$pRBegin]

.label1:
	mov			ecx, esi
	add			ecx, [esp + %$cbWidth]
	sub			ecx, 48 - 3
.label0:
	lddqu		xmm0, [esi   ]			; xmm0 = B5 R4 G4 B4 R3 G3 B3 R2 G2 B2 R1 G1 B1 R0 G0 B0
	lddqu		xmm1, [esi+16]			; xmm1 = Ga Ba R9 G9 B9 R8 G8 B8 R7 G7 B7 R6 G6 B6 R5 G5
	lddqu		xmm2, [esi+32]			; xmm2 = Rf Gf Bf Re Ge Be Rd Gd Bd Rc Gc Bc Rb Gb Bb Ra

	pshufb		xmm0, xmm7				; xmm0 = G4 G3 G2 G1 G0 R4 R3 R2 R1 R0 B5 B4 B3 B2 B1 B0
	pshufb		xmm1, xmm7				; xmm1 = R9 R8 R7 R6 R5 Ba B9 B8 B7 B6 Ga G9 G8 G7 G6 G5
	pshufb		xmm2, xmm7				; xmm2 = Bf Be Bd Bc Bb Gf Ge Gd Gc Gb Rf Re Rd Rc Rb Ra

	movdqa		xmm5, xmm2
	palignr		xmm2, xmm1, 6			; xmm5 = Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 Ba B9 B8 B7 B6
	palignr		xmm1, xmm0, 6			; xmm4 = Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0 R4 R3 R2 R1 R0
	palignr		xmm0, xmm5, 6			; xmm3 = B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb Gf Ge Gd Gc Gb

	movdqa		xmm3, xmm0
	palignr		xmm0, xmm1, 5			; xmm0 = Gf Ge Gd Gc Gb Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
	palignr		xmm1, xmm2, 5			; xmm1 = R4 R3 R2 R1 R0 Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5
	palignr		xmm2, xmm3, 5			; xmm2 = Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb

	movdqu		[edi], xmm0
	psubb		xmm0, xmm6

	palignr		xmm1, xmm1, 11			; xmm1 = Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 R4 R3 R2 R1 R0
	palignr		xmm2, xmm2, 5			; xmm2 = Bf Be Bd Bc Bb Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0

	psubb		xmm1, xmm0
	psubb		xmm2, xmm0
	movdqu		[ebx], xmm2
	movdqu		[edx], xmm1

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
	movdqa		xmm7, [bgrx2planarpshufb16]
%else
	movdqa		xmm7, [xrgb2planarpshufb16]
%endif
	movdqa		xmm6, [_0x80b]
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
	lddqu		xmm0, [esi   ]		; xmm0 = A3 R3 G3 B3 A2 R2 G2 B2 A1 R1 G1 B1 A0 R0 G0 B0 (bgrx)
	lddqu		xmm1, [esi+16]		; xmm1 = A7 R7 G7 B7 A6 R6 G6 B6 A5 R5 G5 B5 A4 R4 G4 B4
	lddqu		xmm2, [esi+32]		; xmm2 = Ab Rb Gb Bb Aa Ra Ga Ba A9 R9 G9 B9 A8 R8 G8 B8
	lddqu		xmm3, [esi+48]		; xmm3 = Af Rf Gf Bf Ae Re Ge Be Ad Rd Gd Bd Ac Rc Gc Bc

	pshufb		xmm0, xmm7			; xmm0 = A3 A2 A1 A0 R3 R2 R1 R0 G3 G2 G1 G0 B3 B2 B1 B0
	pshufb		xmm1, xmm7			; xmm1 = A7 A6 A5 A4 R7 R6 R5 R4 G7 G6 G5 G4 B7 B6 B5 B4
	pshufb		xmm2, xmm7			; xmm2 = Ab Aa A9 A8 Rb Ra R9 R8 Gb Ga G9 G8 Bb Ba B9 B8
	pshufb		xmm3, xmm7			; xmm3 = Af Ae Ad Ac Rf Re Rd Rc Gf Ge Gd Gc Bf Be Bd Bc

	movdqa		xmm4, xmm0
	punpckldq	xmm4, xmm1			; xmm4 = G7 G6 G5 G4 G3 G2 G1 G0 B7 B6 B5 B4 B3 B2 B1 B0
	punpckhdq	xmm0, xmm1			; xmm0 = A7 A6 A5 A4 A3 A2 A1 A0 R7 R6 R5 R4 R3 R2 R1 R0
	movdqa		xmm5, xmm2
	punpckldq	xmm5, xmm3			; xmm5 = Gf Ge Gd Gc Gb Ga G9 G8 Bf Be Bd Bc Bb Ba B9 B8
	punpckhdq	xmm2, xmm3			; xmm2 = Af Ae Ad Ac Ab Aa A9 A8 Rf Re Rd Rc Rb Ra R9 R8

	movdqa		xmm1, xmm4
	punpcklqdq	xmm1, xmm5			; xmm1 = Bf Be Bd Bc Bb Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0
	punpckhqdq	xmm4, xmm5			; xmm4 = Gf Ge Gd Gc Gb Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
%if %$havealpha
	movdqa		xmm3, xmm0
	punpckhqdq	xmm3, xmm2			; xmm3 = Af Ae Ad Ac Ab Aa A9 A8 A7 A6 A5 A4 A3 A2 A1 A0
%endif
	punpcklqdq	xmm0, xmm2			; xmm0 = Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 R4 R3 R2 R1 R0

	movdqu		[edi], xmm4
	psubb		xmm4, xmm6
%if %$havealpha
	movdqu		[ebp], xmm3
%endif

	psubb		xmm1, xmm4
	movdqu		[ebx], xmm1
	psubb		xmm0, xmm4
	movdqu		[edx], xmm0

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

CONVERT_XRGB_TO_ULRGA	_ssse3_ConvertBGRXToULRG, 1, 0
CONVERT_XRGB_TO_ULRGA	_ssse3_ConvertXRGBToULRG, 0, 0
CONVERT_XRGB_TO_ULRGA	_ssse3_ConvertBGRAToULRA, 1, 1
CONVERT_XRGB_TO_ULRGA	_ssse3_ConvertARGBToULRA, 0, 1


	align	64
yuyv2planarpshufb16	dq	0e0c0a0806040200h
					dq	0f0b07030d090501h
planar2yuyvpshufb16	dq	0d0309020c010800h
					dq	0f070b060e050a04h

uyvy2planarpshufb16	dq	0f0d0b0907050301h
					dq	0e0a06020c080400h
planar2uyvypshufb16	dq	030d0209010c0008h
					dq	070f060b050e040ah

%macro CONVERT_YUV422_TO_ULY2 2
%push
	MULTI_CONTEXT_XDEFINE procname, %1, yuyv, %2

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd

%if %$yuyv
	movdqa		xmm7, [yuyv2planarpshufb16]
%else
	movdqa		xmm7, [uyvy2planarpshufb16]
%endif
	mov			esi, [esp + %$pSrcBegin]
	mov			edi, [esp + %$pYBegin]
	mov			ebx, [esp + %$pUBegin]
	mov			edx, [esp + %$pVBegin]

	mov			ecx, [esp + %$pSrcEnd]
	sub			ecx, 64 - 4
.label0:
	lddqu		xmm0, [esi   ]		; xmm0 = V06 Y07 U06 Y06 V04 Y05 U04 Y04 V02 Y03 U02 Y02 V00 Y01 U00 Y00 (yuyv)
	lddqu		xmm1, [esi+16]		; xmm1 = V0e Y0f U0e Y0e V0c Y0d U0c Y0c V0a Y0b U0a Y0a V08 Y09 U08 Y08
	lddqu		xmm2, [esi+32]		; xmm2 = V16 Y17 U16 Y16 V14 Y15 U14 Y14 V12 Y13 U12 Y12 V10 Y11 U10 Y10
	lddqu		xmm3, [esi+48]		; xmm3 = V1e Y1f U1e Y1e V1c Y1d U1c Y1c V1a Y1b U1a Y1a V18 Y19 U18 Y18

	pshufb		xmm0, xmm7			; xmm0 = V06 V04 V02 V00 U06 U04 U02 U00 Y07 Y06 Y05 Y04 Y03 Y02 Y01 Y00
	pshufb		xmm1, xmm7			; xmm1 = V0e V0c V0a V08 U0e U0c U0a U08 Y0f Y0e Y0d Y0c Y0b Y0a Y09 Y08
	pshufb		xmm2, xmm7			; xmm2 = V16 V14 V12 V10 U16 U14 U12 U10 Y17 Y16 Y15 Y14 Y13 Y12 Y11 Y10
	pshufb		xmm3, xmm7			; xmm3 = V1e V1c V1a V18 U1e U1c U1a U18 Y1f Y1e Y1d Y1c Y1b Y1a Y19 Y18

	movdqa		xmm4, xmm0
	movdqa		xmm5, xmm2
	punpcklqdq	xmm4, xmm1			; xmm4 = Y0f Y0e Y0d Y0c Y0b Y0a Y09 Y08 Y07 Y06 Y05 Y04 Y03 Y02 Y01 Y00
	punpcklqdq	xmm5, xmm3			; xmm5 = Y1f Y1e Y1d Y1c Y1b Y1a Y19 Y18 Y17 Y16 Y15 Y14 Y13 Y12 Y11 Y10
	movdqu		[edi   ], xmm4
	movdqu		[edi+16], xmm5

	punpckhdq	xmm0, xmm1			; xmm0 = V0e V0c V0a V08 V06 V04 V02 V00 U0e U0c U0a U08 U06 U04 U02 U00
	punpckhdq	xmm2, xmm3			; xmm2 = V1e V1c V1a V18 V16 V14 V12 V10 U1e U1c U1a U18 U16 U14 U12 U10

	movdqa		xmm1, xmm0
	punpcklqdq	xmm1, xmm2			; xmm1 = U1e U1c U1a U18 U16 U14 U12 U10 U0e U0c U0a U08 U06 U04 U02 U00
	punpckhqdq	xmm0, xmm2			; xmm0 = V1e V1c V1a V18 V16 V14 V12 V10 V0e V0c V0a V08 V06 V04 V02 V00
	movdqu		[ebx], xmm1
	movdqu		[edx], xmm0

	add			edi, 32
	add			ebx, 16
	add			edx, 16
	add			esi, 64
	cmp			esi, ecx
	jb			.label0

	add			ecx, 64 - 4

.label3:
	cmp			esi, ecx
	jae			.label2

	mov			al, [esi+1-%$yuyv]
	mov			[edi  ], al

	mov			al, [esi+3-%$yuyv]
	mov			[edi+1], al

	mov			al, [esi  +%$yuyv]
	mov			[ebx], al

	mov			al, [esi+2+%$yuyv]
	mov			[edx], al

	add			edi, 2
	add			ebx, 1
	add			edx, 1
	add			esi, 4
	jmp			.label3

.label2:

	SIMPLE_EPILOGUE
%pop
%endmacro

CONVERT_YUV422_TO_ULY2	_ssse3_ConvertYUYVToULY2, 1
CONVERT_YUV422_TO_ULY2	_ssse3_ConvertUYVYToULY2, 0



dummyg		dq	0706050403020100h
			dq	0f0e0d0c0b0a0908h
dummyb		dq	9090909090909090h
			dq	9090909090909090h
dummyr		dq	0a0a0a0a0a0a0a0a0h
			dq	0a0a0a0a0a0a0a0a0h
dummya		dq	3736353433323130h
			dq	3f3e3d3c3b3a3938h

%push
global _ssse3_ConvertULRGToBGR
_ssse3_ConvertULRGToBGR:
	SIMPLE_PROLOGUE	0, pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, cbWidth, scbStride

	movdqa		xmm7, [planar2bgrpshufb16]
	movdqa		xmm6, [_0x80b]
	mov			edi, [esp + %$pDstBegin]
	mov			esi, [esp + %$pGBegin]
	mov			ebx, [esp + %$pBBegin]
	mov			edx, [esp + %$pRBegin]

.label1:
	mov			ecx, edi
	add			ecx, [esp + %$cbWidth]
	sub			ecx, 48 - 3
.label0:
	lddqu		xmm0, [esi]				; xmm0 = Gf Ge Gd Gc Gb Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
	lddqu		xmm1, [edx]
	lddqu		xmm2, [ebx]

	movdqa		xmm3, xmm0
	paddb		xmm3, xmm6
	paddb		xmm1, xmm3				; xmm1 = Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 R4 R3 R2 R1 R0
	paddb		xmm2, xmm3				; xmm2 = Bf Be Bd Bc Bb Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0

	palignr		xmm1, xmm1, 5			; xmm1 = R4 R3 R2 R1 R0 Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5
	palignr		xmm2, xmm2, 11			; xmm2 = Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb

	movdqa		xmm5, xmm2
	palignr		xmm2, xmm0, 11			; xmm3 = B5 B4 B3 B2 B1 B0 Bf Be Bd Bc Bb Gf Ge Gd Gc Gb
	palignr		xmm0, xmm1, 11			; xmm4 = Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0 R4 R3 R2 R1 R0
	palignr		xmm1, xmm5, 11			; xmm5 = Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 Ba B9 B8 B7 B6

	movdqa		xmm5, xmm2
	palignr		xmm2, xmm1, 10			; xmm2 = Bf Be Bd Bc Bb Gf Ge Gd Gc Gb Rf Re Rd Rc Rb Ra
	palignr		xmm1, xmm0, 10			; xmm1 = R9 R8 R7 R6 R5 Ba B9 B8 B7 B6 Ga G9 G8 G7 G6 G5
	palignr		xmm0, xmm5, 10			; xmm0 = G4 G3 G2 G1 G0 R4 R3 R2 R1 R0 B5 B4 B3 B2 B1 B0

	pshufb		xmm0, xmm7				; xmm0 = B5 R4 G4 B4 R3 G3 B3 R2 G2 B2 R1 G1 B1 R0 G0 B0
	pshufb		xmm1, xmm7				; xmm1 = Ga Ba R9 G9 B9 R8 G8 B8 R7 G7 B7 R6 G6 B6 R5 G5
	pshufb		xmm2, xmm7				; xmm2 = Rf Gf Bf Re Ge Be Rd Gd Bd Rc Gc Bc Rb Gb Bb Ra

	movdqu		[edi   ], xmm0
	movdqu		[edi+16], xmm1
	movdqu		[edi+32], xmm2

	add			esi, 16
	add			ebx, 16
	add			edx, 16
	add			edi, 48
	cmp			edi, ecx
	jb			.label0

	add			ecx, 48 - 3

.label3:
	cmp			edi, ecx
	jae			.label2
	mov			al, [esi]
	mov			[edi+1], al
	add			al, 80h

	mov			ah, [ebx]
	add			ah, al
	mov			[edi], ah

	mov			ah, [edx]
	add			ah, al
	mov			[edi+2], ah

	add			esi, 1
	add			ebx, 1
	add			edx, 1
	add			edi, 3
	jmp			.label3

.label2:
	sub			edi, [esp + %$cbWidth]
	add			edi, [esp + %$scbStride]
	cmp			edi, [esp + %$pDstEnd]
	jne			.label1

	SIMPLE_EPILOGUE
%pop



%macro CONVERT_ULRGA_TO_XRGB 3
%push
	MULTI_CONTEXT_XDEFINE procname, %1, bgrx, %2, havealpha, %3

global %$procname
%$procname:
%if %$havealpha
	SIMPLE_PROLOGUE 0, pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, pABegin, cbWidth, scbStride
%else
	SIMPLE_PROLOGUE 0, pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, cbWidth, scbStride
%endif

%if %$bgrx
	movdqa		xmm7, [planar2bgrxpshufb16]
%else
	movdqa		xmm7, [planar2xrgbpshufb16]
%endif
	movdqa		xmm6, [_0x80b]
%if ! %$havealpha
	pcmpeqd		xmm3, xmm3
%endif
	mov			edi, [esp + %$pDstBegin]
	mov			esi, [esp + %$pGBegin]
	mov			ebx, [esp + %$pBBegin]
	mov			edx, [esp + %$pRBegin]
%if %$havealpha
	mov			ebp, [esp + %$pABegin]
%endif

.label1:
	mov			ecx, edi
	add			ecx, [esp + %$cbWidth]
	sub			ecx, 64 - 4
.label0:
	lddqu		xmm0, [esi]			; xmm0 = Gf Ge Gd Gc Gb Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
	lddqu		xmm1, [ebx]
	lddqu		xmm2, [edx]
%if %$havealpha
	lddqu		xmm3, [ebp]			; xmm3 = Af Ae Ad Ac Ab Aa A9 A8 A7 A6 A5 A4 A3 A2 A1 A0
%endif
;	lddqu		xmm0, [dummyg]
;	lddqu		xmm1, [dummyb]
;	lddqu		xmm2, [dummyr]

	movdqa		xmm4, xmm0
	paddb		xmm4, xmm6
	paddb		xmm1, xmm4			; xmm1 = Bf Be Bd Bc Bb Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0
	paddb		xmm2, xmm4			; xmm2 = Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 R4 R3 R2 R1 R0

	movdqa		xmm4, xmm1
	punpckldq	xmm4, xmm0			; xmm4 = G7 G6 G5 G4 B7 B6 B5 B4 G3 G2 G1 G0 B3 B2 B1 B0
	punpckhdq	xmm1, xmm0			; xmm1 = Gf Ge Gd Gc Bf Be Bd Bc Gb Ga G9 G8 Bb Ba B9 B8
	movdqa		xmm5, xmm2
	punpckldq	xmm5, xmm3			; xmm5 = A7 A6 A5 A4 R7 R6 R5 R4 A3 A2 A1 A0 R3 R2 R1 R0
	punpckhdq	xmm2, xmm3			; xmm2 = Af Ae Ad Ac Rf Re Rd Rc Ab Aa A9 A8 Rb Ra R9 R8

	movdqa		xmm0, xmm4
	punpcklqdq	xmm0, xmm5			; xmm0 = A3 A2 A1 A0 R3 R2 R1 R0 G3 G2 G1 G0 B3 B2 B1 B0
	punpckhqdq	xmm4, xmm5			; xmm4 = A7 A6 A5 A4 R7 R6 R5 R4 G7 G6 G5 G4 B7 B6 B5 B4
	movdqa		xmm5, xmm1
	punpcklqdq	xmm5, xmm2			; xmm5 = Ab Aa A9 A8 Rb Ra R9 R8 Gb Ga G9 G8 Bb Ba B9 B8
	punpckhqdq	xmm1, xmm2			; xmm1 = Af Ae Ad Ac Rf Re Rd Rc Gf Ge Gd Gc Bf Be Bd Bc

	pshufb		xmm0, xmm7			; xmm0 = A3 R3 G3 B3 A2 R2 G2 B2 A1 R1 G1 B1 A0 R0 G0 B0
	pshufb		xmm4, xmm7			; xmm4 = A7 R7 G7 B7 A6 R6 G6 B6 A5 R5 G5 B5 A4 R4 G4 B4
	pshufb		xmm5, xmm7			; xmm5 = Ab Rb Gb Bb Aa Ra Ga Ba A9 R9 G9 B9 A8 R8 G8 B8
	pshufb		xmm1, xmm7			; xmm1 = Af Rf Gf Bf Ae Re Ge Be Ad Rd Gd Bd Ac Rc Gc Bc

	movdqu		[edi   ], xmm0
	movdqu		[edi+16], xmm4
	movdqu		[edi+32], xmm5
	movdqu		[edi+48], xmm1

	add			esi, 16
	add			ebx, 16
	add			edx, 16
%if %$havealpha
	add			ebp, 16
%endif
	add			edi, 64
	cmp			edi, ecx
	jb			.label0

	add			ecx, 64 - 4

.label3:
	cmp			edi, ecx
	jae			.label2
	mov			al, [esi]
%if %$bgrx
	mov			[edi+1], al
%else
	mov			[edi+2], al
%endif
	sub			al, 80h

%if %$havealpha
	mov			ah, [ebp]
 %if %$bgrx
	mov			[edi+3], ah
 %else
	mov			[edi  ], ah
 %endif
%else
 %if %$bgrx
	mov			byte [edi+3], 0ffh
 %else
	mov			byte [edi  ], 0ffh
 %endif
%endif

	mov			ah, [ebx]
	add			ah, al
%if %$bgrx
	mov			[edi  ], ah
%else
	mov			[edi+3], ah
%endif

	mov			ah, [edx]
	add			ah, al
%if %$bgrx
	mov			[edi+2], ah
%else
	mov			[edi+1], ah
%endif

	add			esi, 1
	add			ebx, 1
	add			edx, 1
%if %$havealpha
	add			ebp, 1
%endif
	add			edi, 4
	jmp			.label3

.label2:
	sub			edi, [esp + %$cbWidth]
	add			edi, [esp + %$scbStride]
	cmp			edi, [esp + %$pDstEnd]
	jne			.label1

	SIMPLE_EPILOGUE
%pop
%endmacro

CONVERT_ULRGA_TO_XRGB	_ssse3_ConvertULRGToBGRX, 1, 0
CONVERT_ULRGA_TO_XRGB	_ssse3_ConvertULRGToXRGB, 0, 0
CONVERT_ULRGA_TO_XRGB	_ssse3_ConvertULRAToBGRA, 1, 1
CONVERT_ULRGA_TO_XRGB	_ssse3_ConvertULRAToARGB, 0, 1



%macro CONVERT_ULY2_TO_YUV422 2
%push
	MULTI_CONTEXT_XDEFINE procname, %1, yuyv, %2

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin

%if %$yuyv
	movdqa		xmm7, [planar2yuyvpshufb16]
%else
	movdqa		xmm7, [planar2uyvypshufb16]
%endif
	mov			edi, [esp + %$pDstBegin]
	mov			esi, [esp + %$pYBegin]
	mov			ebx, [esp + %$pUBegin]
	mov			edx, [esp + %$pVBegin]

	mov			ecx, [esp + %$pDstEnd]
	sub			ecx, 64 - 4
.label0:
	lddqu		xmm2, [ebx]			; xmm2 = U1e U1c U1a U18 U16 U14 U12 U10 U0e U0c U0a U08 U06 U04 U02 U00
	lddqu		xmm3, [edx]			; xmm3 = V1e V1c V1a V18 V16 V14 V12 V10 V0e V0c V0a V08 V06 V04 V02 V00
	lddqu		xmm0, [esi   ]		; xmm0 = Y0f Y0e Y0d Y0c Y0b Y0a Y09 Y08 Y07 Y06 Y05 Y04 Y03 Y02 Y01 Y00
	lddqu		xmm1, [esi+16]		; xmm1 = Y1f Y1e Y1d Y1c Y1b Y1a Y19 Y18 Y17 Y16 Y15 Y14 Y13 Y12 Y11 Y10

	movdqa		xmm4, xmm2
	punpckldq	xmm4, xmm3			; xmm4 = V0e V0c V0a V08 U0e U0c U0a U08 V06 V04 V02 V00 U06 U04 U02 U00
	punpckhdq	xmm2, xmm3			; xmm2 = V1e V1c V1a V18 U1e U1c U1a U18 V16 V14 V12 V10 U16 U14 U12 U10

	movdqa		xmm5, xmm0
	punpcklqdq	xmm5, xmm4			; xmm5 = V06 V04 V02 V00 U06 U04 U02 U00 Y07 Y06 Y05 Y04 Y03 Y02 Y01 Y00
	punpckhqdq	xmm0, xmm4			; xmm0 = V0e V0c V0a V08 U0e U0c U0a U08 Y0f Y0e Y0d Y0c Y0b Y0a Y09 Y08
	movdqa		xmm4, xmm1
	punpcklqdq	xmm4, xmm2			; xmm4 = V16 V14 V12 V10 U16 U14 U12 U10 Y17 Y16 Y15 Y14 Y13 Y12 Y11 Y10
	punpckhqdq	xmm1, xmm2			; xmm1 = V1e V1c V1a V18 U1e U1c U1a U18 Y1f Y1e Y1d Y1c Y1b Y1a Y19 Y18

	pshufb		xmm5, xmm7			; xmm5 = V06 Y07 U06 Y06 V04 Y05 U04 Y04 V02 Y03 U02 Y02 V00 Y01 U00 Y00
	pshufb		xmm0, xmm7			; xmm0 = V0e Y0f U0e Y0e V0c Y0d U0c Y0c V0a Y0b U0a Y0a V08 Y09 U08 Y08
	pshufb		xmm4, xmm7			; xmm4 = V16 Y17 U16 Y16 V14 Y15 U14 Y14 V12 Y13 U12 Y12 V10 Y11 U10 Y10
	pshufb		xmm1, xmm7			; xmm1 = V1e Y1f U1e Y1e V1c Y1d U1c Y1c V1a Y1b U1a Y1a V18 Y19 U18 Y18

	movdqu		[edi   ], xmm5
	movdqu		[edi+16], xmm0
	movdqu		[edi+32], xmm4
	movdqu		[edi+48], xmm1

	add			esi, 32
	add			ebx, 16
	add			edx, 16
	add			edi, 64
	cmp			edi, ecx
	jb			.label0

	add			ecx, 64 - 4

.label3:
	cmp			edi, ecx
	jae			.label2

	mov			al, [esi  ]
	mov			[edi+1-%$yuyv],al

	mov			al, [esi+1]
	mov			[edi+3-%$yuyv], al

	mov			al, [ebx]
	mov			[edi  +%$yuyv], al

	mov			al, [edx]
	mov			[edi+2+%$yuyv], al

	add			esi, 2
	add			ebx, 1
	add			edx, 1
	add			edi, 4
	jmp			.label3

.label2:

	SIMPLE_EPILOGUE
%pop
%endmacro

CONVERT_ULY2_TO_YUV422	_ssse3_ConvertULY2ToYUYV, 1
CONVERT_ULY2_TO_YUV422	_ssse3_ConvertULY2ToUYVY, 0
