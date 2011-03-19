; •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e
; $Id$

STD_PROLOG	macro	_numlocal
numlocal = &_numlocal
argsoffset = numlocal + 4 * 4 + 4
	push		ebx
	push		esi
	push		edi
	push		ebp
if numlocal ne 0
	sub			esp, numlocal
endif
endm

STD_EPILOG	macro
if numlocal ne 0
	add			esp, numlocal
endif
	pop			ebp
	pop			edi
	pop			esi
	pop			ebx
endm	
