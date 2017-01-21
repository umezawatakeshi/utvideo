; ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe
; $Id$


%include "Common_asm_x64.mac"


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
global ssse3_ConvertBGRToULRG
ssse3_ConvertBGRToULRG:
	SIMPLE_PROLOGUE	0, pGBegin, pBBegin, pRBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride

	movdqa		xmm7, [bgr2planarpshufb16]
	movdqa		xmm6, [_0x80b]
	mov			rsi, [rsp + %$pSrcBegin]
	mov			rdi, [rsp + %$pGBegin]
	mov			rbx, [rsp + %$pBBegin]
	mov			rdx, [rsp + %$pRBegin]
	mov			r9,  [rsp + %$pSrcEnd]
	mov			r10, [rsp + %$cbWidth]
	mov			r11, [rsp + %$scbStride]
	sub			r11, r10
	sub			r10, 48 - 3
	xor			rcx, rcx

.label1:
	lea			r8, [rsi+r10]
.label0:
	lddqu		xmm0, [rsi   ]			; xmm0 = B5 R4 G4 B4 R3 G3 B3 R2 G2 B2 R1 G1 B1 R0 G0 B0
	lddqu		xmm1, [rsi+16]			; xmm1 = Ga Ba R9 G9 B9 R8 G8 B8 R7 G7 B7 R6 G6 B6 R5 G5
	lddqu		xmm2, [rsi+32]			; xmm2 = Rf Gf Bf Re Ge Be Rd Gd Bd Rc Gc Bc Rb Gb Bb Ra

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

	movdqu		[rdi+rcx], xmm0
	psubb		xmm0, xmm6

	palignr		xmm1, xmm1, 11			; xmm1 = Rf Re Rd Rc Rb Ra R9 R8 R7 R6 R5 R4 R3 R2 R1 R0
	palignr		xmm2, xmm2, 5			; xmm2 = Bf Be Bd Bc Bb Ba B9 B8 B7 B6 B5 B4 B3 B2 B1 B0

	psubb		xmm1, xmm0
	psubb		xmm2, xmm0
	movdqu		[rbx+rcx], xmm2
	movdqu		[rdx+rcx], xmm1

	add			rcx, 16
	add			rsi, 48
	cmp			rsi, r8
	jb			.label0

	add			r8, 48 - 3

.label3:
	cmp			rsi, r8
	jae			.label2
	mov			al, [rsi+1]
	mov			[rdi+rcx], al
	sub			al, 80h

	mov			ah, [rsi  ]
	sub			ah, al
	mov			[rbx+rcx], ah

	mov			ah, [rsi+2]
	sub			ah, al
	mov			[rdx+rcx], ah

	add			rcx, 1
	add			rsi, 3
	jmp			.label3

.label2:
	add			rsi, r11
	cmp			rsi, r9
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
	mov			rsi, [rsp + %$pSrcBegin]
	mov			rdi, [rsp + %$pGBegin]
	mov			rbx, [rsp + %$pBBegin]
	mov			rdx, [rsp + %$pRBegin]
%if %$havealpha
	mov			rbp, [rsp + %$pABegin]
%endif
	mov			r9,  [rsp + %$pSrcEnd]
	mov			r10, [rsp + %$cbWidth]
	mov			r11, [rsp + %$scbStride]
	sub			r11, r10
	sub			r10, 64 - 4
	xor			rcx, rcx

.label1:
	lea			r8, [rsi+r10]
.label0:
	lddqu		xmm0, [rsi   ]		; xmm0 = A3 R3 G3 B3 A2 R2 G2 B2 A1 R1 G1 B1 A0 R0 G0 B0 (bgrx)
	lddqu		xmm1, [rsi+16]		; xmm1 = A7 R7 G7 B7 A6 R6 G6 B6 A5 R5 G5 B5 A4 R4 G4 B4
	lddqu		xmm2, [rsi+32]		; xmm2 = Ab Rb Gb Bb Aa Ra Ga Ba A9 R9 G9 B9 A8 R8 G8 B8
	lddqu		xmm3, [rsi+48]		; xmm3 = Af Rf Gf Bf Ae Re Ge Be Ad Rd Gd Bd Ac Rc Gc Bc

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

	movdqu		[rdi+rcx], xmm4
	psubb		xmm4, xmm6
%if %$havealpha
	movdqu		[rbp+rcx], xmm3
%endif

	psubb		xmm1, xmm4
	movdqu		[rbx+rcx], xmm1
	psubb		xmm0, xmm4
	movdqu		[rdx+rcx], xmm0

	add			rcx, 16
	add			rsi, 64
	cmp			rsi, r8
	jb			.label0

	add			r8, 64 - 4

.label3:
	cmp			rsi, r8
	jae			.label2

	mov			al, [rsi+2-%$bgrx]
	mov			[rdi+rcx], al
	sub			al, 80h

%if %$havealpha
	mov			ah, [rsi+%$bgrx*3]
	mov			[rbp+rcx], ah
%endif

	mov			ah, [rsi+3-%$bgrx*3]
	sub			ah, al
	mov			[rbx+rcx], ah

	mov			ah, [rsi+1+%$bgrx]
	sub			ah, al
	mov			[rdx+rcx], ah

	add			rcx, 1
	add			rsi, 4
	jmp			.label3

.label2:
	add			rsi, r11
	cmp			rsi, r9
	jne			.label1

	SIMPLE_EPILOGUE
%pop
%endmacro

CONVERT_XRGB_TO_ULRGA	ssse3_ConvertBGRXToULRG, 1, 0
CONVERT_XRGB_TO_ULRGA	ssse3_ConvertXRGBToULRG, 0, 0
CONVERT_XRGB_TO_ULRGA	ssse3_ConvertBGRAToULRA, 1, 1
CONVERT_XRGB_TO_ULRGA	ssse3_ConvertARGBToULRA, 0, 1


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
	SIMPLE_PROLOGUE 0, pYBegin, pUBegin, pVBegin, pSrcBegin, pSrcEnd, cbWidth, scbStride

%if %$yuyv
	movdqa		xmm7, [yuyv2planarpshufb16]
%else
	movdqa		xmm7, [uyvy2planarpshufb16]
%endif
	mov			rsi, [rsp + %$pSrcBegin]
	mov			rdi, [rsp + %$pYBegin]
	mov			rbx, [rsp + %$pUBegin]
	mov			rdx, [rsp + %$pVBegin]
	mov			r9,  [rsp + %$pSrcEnd]
	mov			r10, [rsp + %$cbWidth]
	mov			r11, [rsp + %$scbStride]
	sub			r11, r10
	sub			r10, 64 - 4
	xor			rcx, rcx

.label1:
	lea			r8, [rsi+r10]
.label0:
	lddqu		xmm0, [rsi   ]		; xmm0 = V06 Y07 U06 Y06 V04 Y05 U04 Y04 V02 Y03 U02 Y02 V00 Y01 U00 Y00 (yuyv)
	lddqu		xmm1, [rsi+16]		; xmm1 = V0e Y0f U0e Y0e V0c Y0d U0c Y0c V0a Y0b U0a Y0a V08 Y09 U08 Y08
	lddqu		xmm2, [rsi+32]		; xmm2 = V16 Y17 U16 Y16 V14 Y15 U14 Y14 V12 Y13 U12 Y12 V10 Y11 U10 Y10
	lddqu		xmm3, [rsi+48]		; xmm3 = V1e Y1f U1e Y1e V1c Y1d U1c Y1c V1a Y1b U1a Y1a V18 Y19 U18 Y18

	pshufb		xmm0, xmm7			; xmm0 = V06 V04 V02 V00 U06 U04 U02 U00 Y07 Y06 Y05 Y04 Y03 Y02 Y01 Y00
	pshufb		xmm1, xmm7			; xmm1 = V0e V0c V0a V08 U0e U0c U0a U08 Y0f Y0e Y0d Y0c Y0b Y0a Y09 Y08
	pshufb		xmm2, xmm7			; xmm2 = V16 V14 V12 V10 U16 U14 U12 U10 Y17 Y16 Y15 Y14 Y13 Y12 Y11 Y10
	pshufb		xmm3, xmm7			; xmm3 = V1e V1c V1a V18 U1e U1c U1a U18 Y1f Y1e Y1d Y1c Y1b Y1a Y19 Y18

	movdqa		xmm4, xmm0
	movdqa		xmm5, xmm2
	punpcklqdq	xmm4, xmm1			; xmm4 = Y0f Y0e Y0d Y0c Y0b Y0a Y09 Y08 Y07 Y06 Y05 Y04 Y03 Y02 Y01 Y00
	punpcklqdq	xmm5, xmm3			; xmm5 = Y1f Y1e Y1d Y1c Y1b Y1a Y19 Y18 Y17 Y16 Y15 Y14 Y13 Y12 Y11 Y10
	movdqu		[rdi+rcx*2   ], xmm4
	movdqu		[rdi+rcx*2+16], xmm5

	punpckhdq	xmm0, xmm1			; xmm0 = V0e V0c V0a V08 V06 V04 V02 V00 U0e U0c U0a U08 U06 U04 U02 U00
	punpckhdq	xmm2, xmm3			; xmm2 = V1e V1c V1a V18 V16 V14 V12 V10 U1e U1c U1a U18 U16 U14 U12 U10

	movdqa		xmm1, xmm0
	punpcklqdq	xmm1, xmm2			; xmm1 = U1e U1c U1a U18 U16 U14 U12 U10 U0e U0c U0a U08 U06 U04 U02 U00
	punpckhqdq	xmm0, xmm2			; xmm0 = V1e V1c V1a V18 V16 V14 V12 V10 V0e V0c V0a V08 V06 V04 V02 V00
	movdqu		[rbx+rcx], xmm1
	movdqu		[rdx+rcx], xmm0

	add			rcx, 16
	add			rsi, 64
	cmp			rsi, r8
	jb			.label0

	add			r8, 64 - 4

.label3:
	cmp			rsi, r8
	jae			.label2

	mov			al, [rsi+1-%$yuyv]
	mov			[rdi+rcx*2  ], al

	mov			al, [esi+3-%$yuyv]
	mov			[rdi+rcx*2+1], al

	mov			al, [esi  +%$yuyv]
	mov			[rbx+rcx], al

	mov			al, [esi+2+%$yuyv]
	mov			[rdx+rcx], al

	add			rcx, 1
	add			rsi, 4
	jmp			.label3

.label2:
	add			rsi, r11
	cmp			rsi, r9
	jne			.label1

	SIMPLE_EPILOGUE
%pop
%endmacro

CONVERT_YUV422_TO_ULY2	ssse3_ConvertYUYVToULY2, 1
CONVERT_YUV422_TO_ULY2	ssse3_ConvertUYVYToULY2, 0



dummyg		dq	0706050403020100h
			dq	0f0e0d0c0b0a0908h
dummyb		dq	9090909090909090h
			dq	9090909090909090h
dummyr		dq	0a0a0a0a0a0a0a0a0h
			dq	0a0a0a0a0a0a0a0a0h
dummya		dq	3736353433323130h
			dq	3f3e3d3c3b3a3938h

%push
global ssse3_ConvertULRGToBGR
ssse3_ConvertULRGToBGR:
	SIMPLE_PROLOGUE	0, pDstBegin, pDstEnd, pGBegin, pBBegin, pRBegin, cbWidth, scbStride

	movdqa		xmm7, [planar2bgrpshufb16]
	movdqa		xmm6, [_0x80b]
	mov			rdi, [rsp + %$pDstBegin]
	mov			rsi, [rsp + %$pGBegin]
	mov			rbx, [rsp + %$pBBegin]
	mov			rdx, [rsp + %$pRBegin]
	mov			r9,  [rsp + %$pDstEnd]
	mov			r10, [rsp + %$cbWidth]
	mov			r11, [rsp + %$scbStride]
	sub			r11, r10
	sub			r10, 48 - 3
	xor			rcx, rcx

.label1:
	lea			r8, [rdi+r10]
.label0:
	lddqu		xmm0, [rsi+rcx]			; xmm0 = Gf Ge Gd Gc Gb Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
	lddqu		xmm1, [rdx+rcx]
	lddqu		xmm2, [rbx+rcx]

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

	movdqu		[rdi   ], xmm0
	movdqu		[rdi+16], xmm1
	movdqu		[rdi+32], xmm2

	add			rcx, 16
	add			rdi, 48
	cmp			rdi, r8
	jb			.label0

	add			r8, 48 - 3

.label3:
	cmp			rdi, r8
	jae			.label2
	mov			al, [rsi+rcx]
	mov			[rdi+1], al
	add			al, 80h

	mov			ah, [rbx+rcx]
	add			ah, al
	mov			[rdi], ah

	mov			ah, [rdx+rcx]
	add			ah, al
	mov			[rdi+2], ah

	add			rcx, 1
	add			rdi, 3
	jmp			.label3

.label2:
	add			rdi, r11
	cmp			rdi, r9
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
	mov			rdi, [rsp + %$pDstBegin]
	mov			rsi, [rsp + %$pGBegin]
	mov			rbx, [rsp + %$pBBegin]
	mov			rdx, [rsp + %$pRBegin]
%if %$havealpha
	mov			rbp, [rsp + %$pABegin]
%endif
	mov			r9,  [rsp + %$pDstEnd]
	mov			r10, [rsp + %$cbWidth]
	mov			r11, [rsp + %$scbStride]
	sub			r11, r10
	sub			r10, 64 - 4
	xor			rcx, rcx

.label1:
	lea			r8, [rdi+r10]
.label0:
	lddqu		xmm0, [rsi+rcx]		; xmm0 = Gf Ge Gd Gc Gb Ga G9 G8 G7 G6 G5 G4 G3 G2 G1 G0
	lddqu		xmm1, [rbx+rcx]
	lddqu		xmm2, [rdx+rcx]
%if %$havealpha
	lddqu		xmm3, [rbp+rcx]		; xmm3 = Af Ae Ad Ac Ab Aa A9 A8 A7 A6 A5 A4 A3 A2 A1 A0
%endif
;	vlddqu		xmm0, [dummyg]
;	vlddqu		xmm1, [dummyb]
;	vlddqu		xmm2, [dummyr]

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

	movdqu		[rdi   ], xmm0
	movdqu		[rdi+16], xmm4
	movdqu		[rdi+32], xmm5
	movdqu		[rdi+48], xmm1

	add			rcx, 16
	add			rdi, 64
	cmp			rdi, r8
	jb			.label0

	add			r8, 64 - 4

.label3:
	cmp			rdi, r8
	jae			.label2
	mov			al, [rsi+rcx]
%if %$bgrx
	mov			[rdi+1], al
%else
	mov			[rdi+2], al
%endif
	sub			al, 80h

%if %$havealpha
	mov			ah, [rbp+rcx]
 %if %$bgrx
	mov			[rdi+3], ah
 %else
	mov			[rdi  ], ah
 %endif
%else
 %if %$bgrx
	mov			byte [rdi+3], 0ffh
 %else
	mov			byte [rdi  ], 0ffh
 %endif
%endif

	mov			ah, [rbx+rcx]
	add			ah, al
%if %$bgrx
	mov			[rdi  ], ah
%else
	mov			[rdi+3], ah
%endif

	mov			ah, [rdx+rcx]
	add			ah, al
%if %$bgrx
	mov			[rdi+2], ah
%else
	mov			[rdi+1], ah
%endif

	add			rcx, 1
	add			rdi, 4
	jmp			.label3

.label2:
	add			rdi, r11
	cmp			rdi, r9
	jne			.label1

	SIMPLE_EPILOGUE
%pop
%endmacro

CONVERT_ULRGA_TO_XRGB	ssse3_ConvertULRGToBGRX, 1, 0
CONVERT_ULRGA_TO_XRGB	ssse3_ConvertULRGToXRGB, 0, 0
CONVERT_ULRGA_TO_XRGB	ssse3_ConvertULRAToBGRA, 1, 1
CONVERT_ULRGA_TO_XRGB	ssse3_ConvertULRAToARGB, 0, 1



%macro CONVERT_ULY2_TO_YUV422 2
%push
	MULTI_CONTEXT_XDEFINE procname, %1, yuyv, %2

global %$procname
%$procname:
	SIMPLE_PROLOGUE 0, pDstBegin, pDstEnd, pYBegin, pUBegin, pVBegin, cbWidth, scbStride

%if %$yuyv
	movdqa		xmm7, [planar2yuyvpshufb16]
%else
	movdqa		xmm7, [planar2uyvypshufb16]
%endif
	mov			rdi, [rsp + %$pDstBegin]
	mov			rsi, [rsp + %$pYBegin]
	mov			rbx, [rsp + %$pUBegin]
	mov			rdx, [rsp + %$pVBegin]
	mov			r9,  [rsp + %$pDstEnd]
	mov			r10, [rsp + %$cbWidth]
	mov			r11, [rsp + %$scbStride]
	sub			r11, r10
	sub			r10, 64 - 4
	xor			rcx, rcx

.label1:
	lea			r8, [rdi+r10]
.label0:
	lddqu		xmm2, [rbx+rcx]			; xmm2 = U1e U1c U1a U18 U16 U14 U12 U10 U0e U0c U0a U08 U06 U04 U02 U00
	lddqu		xmm3, [rdx+rcx]			; xmm3 = V1e V1c V1a V18 V16 V14 V12 V10 V0e V0c V0a V08 V06 V04 V02 V00
	lddqu		xmm0, [rsi+rcx*2   ]	; xmm0 = Y0f Y0e Y0d Y0c Y0b Y0a Y09 Y08 Y07 Y06 Y05 Y04 Y03 Y02 Y01 Y00
	lddqu		xmm1, [rsi+rcx*2+16]	; xmm1 = Y1f Y1e Y1d Y1c Y1b Y1a Y19 Y18 Y17 Y16 Y15 Y14 Y13 Y12 Y11 Y10

	movdqa		xmm4, xmm2
	punpckldq	xmm4, xmm3				; xmm4 = V0e V0c V0a V08 U0e U0c U0a U08 V06 V04 V02 V00 U06 U04 U02 U00
	punpckhdq	xmm2, xmm3				; xmm2 = V1e V1c V1a V18 U1e U1c U1a U18 V16 V14 V12 V10 U16 U14 U12 U10

	movdqa		xmm5, xmm0
	punpcklqdq	xmm5, xmm4				; xmm5 = V06 V04 V02 V00 U06 U04 U02 U00 Y07 Y06 Y05 Y04 Y03 Y02 Y01 Y00
	punpckhqdq	xmm0, xmm4				; xmm0 = V0e V0c V0a V08 U0e U0c U0a U08 Y0f Y0e Y0d Y0c Y0b Y0a Y09 Y08
	movdqa		xmm4, xmm1
	punpcklqdq	xmm4, xmm2				; xmm4 = V16 V14 V12 V10 U16 U14 U12 U10 Y17 Y16 Y15 Y14 Y13 Y12 Y11 Y10
	punpckhqdq	xmm1, xmm2				; xmm1 = V1e V1c V1a V18 U1e U1c U1a U18 Y1f Y1e Y1d Y1c Y1b Y1a Y19 Y18

	pshufb		xmm5, xmm7				; xmm5 = V06 Y07 U06 Y06 V04 Y05 U04 Y04 V02 Y03 U02 Y02 V00 Y01 U00 Y00
	pshufb		xmm0, xmm7				; xmm0 = V0e Y0f U0e Y0e V0c Y0d U0c Y0c V0a Y0b U0a Y0a V08 Y09 U08 Y08
	pshufb		xmm4, xmm7				; xmm4 = V16 Y17 U16 Y16 V14 Y15 U14 Y14 V12 Y13 U12 Y12 V10 Y11 U10 Y10
	pshufb		xmm1, xmm7				; xmm1 = V1e Y1f U1e Y1e V1c Y1d U1c Y1c V1a Y1b U1a Y1a V18 Y19 U18 Y18

	movdqu		[rdi   ], xmm5
	movdqu		[rdi+16], xmm0
	movdqu		[rdi+32], xmm4
	movdqu		[rdi+48], xmm1

	add			rcx, 16
	add			rdi, 64
	cmp			rdi, r8
	jb			.label0

	add			r8, 64 - 4

.label3:
	cmp			rdi, r8
	jae			.label2

	mov			al, [rsi+rcx*2  ]
	mov			[rdi+1-%$yuyv],al

	mov			al, [rsi+rcx*2+1]
	mov			[rdi+3-%$yuyv], al

	mov			al, [rbx+rcx]
	mov			[rdi  +%$yuyv], al

	mov			al, [rdx+rcx]
	mov			[rdi+2+%$yuyv], al

	add			rcx, 1
	add			rdi, 4
	jmp			.label3

.label2:
	add			rdi, r11
	cmp			rdi, r9
	jne			.label1

	SIMPLE_EPILOGUE
%pop
%endmacro

CONVERT_ULY2_TO_YUV422	ssse3_ConvertULY2ToYUYV, 1
CONVERT_ULY2_TO_YUV422	ssse3_ConvertULY2ToUYVY, 0
