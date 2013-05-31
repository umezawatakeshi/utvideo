/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#pragma once

/*
 * Y'  = Kr R' + (1 - Kr - Kb) G' + Kb B'
 * Cb' = (B' - Y') / 2 (1 - Kb) = (-Kr / 2 (1 - Kb)) R' + ((Kr + Kb - 1) / 2 (1 - Kb)) G' +                0.5 B'
 * Cr' = (R' - Y') / 2 (1 - Kr) =                0.5 R' + ((Kr + Kb - 1) / 2 (1 - Kr)) G' + (-Kb / 2 (1 - Kr)) B'
 *
 * R'  = Y' + 2 (1 - Kr) Cr'
 * B'  = Y' + 2 (1 - Kb) Cb'
 * G'  = Y' + (2 (1 - Kb) Kb / (1 - Kr - Kb)) Cb' + (2 (1 - Kr) Kr / (1 - Kr - Kb)) Cr'
 */

/*
 * Recommendation ITU-R BT.601
 * Studio encoding parameters of digital television for standard 4:3 and wide screen 16:9 aspect ratios
 * http://www.itu.int/rec/R-REC-BT.601/
 */
#define COEFF_BT601_KR 0.299
#define COEFF_BT601_KB 0.114

/*
 * Recommendation ITU-R BT.709
 * Parameter values for the HDTV standards for production and international programme exchange
 * http://www.itu.int/rec/R-REC-BT.709/
 */
#define COEFF_BT709_KR 0.2126
#define COEFF_BT709_KB 0.0722


#define COEFF_RGB2Y8(K) ((K) * (235 - 16) / 255)
#define COEFF_RGB2C8(K) ((K) * (240 - 16) / 255)

#define COEFF_Y2RGB8    (1.0 * 255 / (235 - 16))
#define COEFF_C2RGB8(K) ((K) * 255 / (240 - 16))


#define COEFF_R2Y8(Kr, Kb) COEFF_RGB2Y8(Kr)
#define COEFF_G2Y8(Kr, Kb) COEFF_RGB2Y8(1.0 - Kr - Kb)
#define COEFF_B2Y8(Kr, Kb) COEFF_RGB2Y8(Kb)

#define COEFF_R2U8(Kr, Kb) COEFF_RGB2C8(-Kr              / (1.0 - Kb) / 2.0)
#define COEFF_G2U8(Kr, Kb) COEFF_RGB2C8(-(1.0 - Kr - Kb) / (1.0 - Kb) / 2.0)
#define COEFF_B2U8(Kr, Kb) COEFF_RGB2C8(0.5)

#define COEFF_R2V8(Kr, Kb) COEFF_RGB2C8(0.5)
#define COEFF_G2V8(Kr, Kb) COEFF_RGB2C8(-(1.0 - Kr - Kb) / (1.0 - Kr) / 2.0)
#define COEFF_B2V8(Kr, Kb) COEFF_RGB2C8(-Kb              / (1.0 - Kr) / 2.0)

#define COEFF_U2B8(Kr, Kb) COEFF_C2RGB8( 2.0 * (1.0 - Kb))
#define COEFF_V2R8(Kr, Kb) COEFF_C2RGB8( 2.0 * (1.0 - Kr))
#define COEFF_U2G8(Kr, Kb) COEFF_C2RGB8(-2.0 * (1.0 - Kb) * Kb / (1.0 - Kr - Kb))
#define COEFF_V2G8(Kr, Kb) COEFF_C2RGB8(-2.0 * (1.0 - Kr) * Kr / (1.0 - Kr - Kb))


class CBT601Coefficient
{
public:
	/*
	 * Visual C++ (2005-2012) では、以下の宣言は
	 * 「スタティック const 整数データ メンバ以外をクラス内で初期化することはできません」
	 * というエラーになる。
	 * 次善の策として定数関数として定義する。
	 */
	//static const float R2Y = COEFF_R2Y8(COEFF_BT601_KR, COEFF_BT601_KB);
	static inline float R2Y(void)   { return (float)COEFF_R2Y8(COEFF_BT601_KR, COEFF_BT601_KB); }
	static inline float G2Y(void)   { return (float)COEFF_G2Y8(COEFF_BT601_KR, COEFF_BT601_KB); }
	static inline float B2Y(void)   { return (float)COEFF_B2Y8(COEFF_BT601_KR, COEFF_BT601_KB); }
	static inline float R2U(void)   { return (float)COEFF_R2U8(COEFF_BT601_KR, COEFF_BT601_KB); }
	static inline float G2U(void)   { return (float)COEFF_G2U8(COEFF_BT601_KR, COEFF_BT601_KB); }
	static inline float B2U(void)   { return (float)COEFF_B2U8(COEFF_BT601_KR, COEFF_BT601_KB); }
	static inline float R2V(void)   { return (float)COEFF_R2V8(COEFF_BT601_KR, COEFF_BT601_KB); }
	static inline float G2V(void)   { return (float)COEFF_G2V8(COEFF_BT601_KR, COEFF_BT601_KB); }
	static inline float B2V(void)   { return (float)COEFF_B2V8(COEFF_BT601_KR, COEFF_BT601_KB); }

	static inline float Y2RGB(void) { return (float)COEFF_Y2RGB8; }
	static inline float U2B(void)   { return (float)COEFF_U2B8(COEFF_BT601_KR, COEFF_BT601_KB); }
	static inline float V2R(void)   { return (float)COEFF_V2R8(COEFF_BT601_KR, COEFF_BT601_KB); }
	static inline float U2G(void)   { return (float)COEFF_U2G8(COEFF_BT601_KR, COEFF_BT601_KB); }
	static inline float V2G(void)   { return (float)COEFF_V2G8(COEFF_BT601_KR, COEFF_BT601_KB); }
};

class CBT709Coefficient
{
public:
	static inline float R2Y(void)   { return (float)COEFF_R2Y8(COEFF_BT709_KR, COEFF_BT709_KB); }
	static inline float G2Y(void)   { return (float)COEFF_G2Y8(COEFF_BT709_KR, COEFF_BT709_KB); }
	static inline float B2Y(void)   { return (float)COEFF_B2Y8(COEFF_BT709_KR, COEFF_BT709_KB); }
	static inline float R2U(void)   { return (float)COEFF_R2U8(COEFF_BT709_KR, COEFF_BT709_KB); }
	static inline float G2U(void)   { return (float)COEFF_G2U8(COEFF_BT709_KR, COEFF_BT709_KB); }
	static inline float B2U(void)   { return (float)COEFF_B2U8(COEFF_BT709_KR, COEFF_BT709_KB); }
	static inline float R2V(void)   { return (float)COEFF_R2V8(COEFF_BT709_KR, COEFF_BT709_KB); }
	static inline float G2V(void)   { return (float)COEFF_G2V8(COEFF_BT709_KR, COEFF_BT709_KB); }
	static inline float B2V(void)   { return (float)COEFF_B2V8(COEFF_BT709_KR, COEFF_BT709_KB); }

	static inline float Y2RGB(void) { return (float)COEFF_Y2RGB8; }
	static inline float U2B(void)   { return (float)COEFF_U2B8(COEFF_BT709_KR, COEFF_BT709_KB); }
	static inline float V2R(void)   { return (float)COEFF_V2R8(COEFF_BT709_KR, COEFF_BT709_KB); }
	static inline float U2G(void)   { return (float)COEFF_U2G8(COEFF_BT709_KR, COEFF_BT709_KB); }
	static inline float V2G(void)   { return (float)COEFF_V2G8(COEFF_BT709_KR, COEFF_BT709_KB); }
};


#define R2Y   R2Y()
#define G2Y   G2Y()
#define B2Y   B2Y()
#define R2U   R2U()
#define G2U   G2U()
#define B2U   B2U()
#define R2V   R2V()
#define G2V   G2V()
#define B2V   B2V()
#define Y2RGB Y2RGB()
#define U2B   U2B()
#define V2R   V2R()
#define U2G   U2G()
#define V2G   V2G()
