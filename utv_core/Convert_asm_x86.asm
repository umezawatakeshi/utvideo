; ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe
; $Id$


%include "Common_asm_x86.mac"


section .text


	align	64
bgr2planarpshufb16	dq	05020f0c09060300h
					dq	0d0a0704010e0b08h
planar2bgrpshufb16	dq	0d02070c01060b00h
					dq	050a0f04090e0308h

bgrx2planarpshufb16	dq	0d0905010c080400h
					dq	0f0b07030e0a0602h
planar2bgrxpshufb16	dq	0d0905010c080400h	; åvéZÇµÇƒÇ›ÇΩÇÁé¿ÇÕìØÇ∂ÇæÇ∆Ç¢Ç§Åc
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

	mov			al, [esi+2-%$bgrx]
	mov			[edi], al
	sub			al, 80h

%if %$havealpha
	mov			ah, [esi+%$bgrx*3]
	mov			[ebp], ah
%endif

	mov			ah, [esi+3-%$bgrx*3]
	sub			ah, al
	mov			[ebx], ah

	mov			ah, [esi+1+%$bgrx]
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
