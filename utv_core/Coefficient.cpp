/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

/* This file is NOT a part of "utv_core" module. */

#include <stdio.h>
#include "Coefficient.h"

// for NASM
#define PRINT(CN, V) printf("%s_%s	equ	%+9d ; %+f\n", #CN, #V, int(C ## CN ## Coefficient::V * (1 << 24)), C ## CN ## Coefficient::V);

int main(void)
{
	PRINT(BT601, R2Y);
	PRINT(BT601, G2Y);
	PRINT(BT601, B2Y);
	PRINT(BT601, R2U);
	PRINT(BT601, G2U);
	PRINT(BT601, B2U);
	PRINT(BT601, R2V);
	PRINT(BT601, G2V);
	PRINT(BT601, B2V);
	PRINT(BT601, Y2RGB);
	PRINT(BT601, U2B);
	PRINT(BT601, V2R);
	PRINT(BT601, U2G);
	PRINT(BT601, V2G);

	printf("\n");

	PRINT(BT709, R2Y);
	PRINT(BT709, G2Y);
	PRINT(BT709, B2Y);
	PRINT(BT709, R2U);
	PRINT(BT709, G2U);
	PRINT(BT709, B2U);
	PRINT(BT709, R2V);
	PRINT(BT709, G2V);
	PRINT(BT709, B2V);
	PRINT(BT709, Y2RGB);
	PRINT(BT709, U2B);
	PRINT(BT709, V2R);
	PRINT(BT709, U2G);
	PRINT(BT709, V2G);
}
