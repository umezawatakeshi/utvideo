/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
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


// ÉeÉìÉvÉåÅ[Égà¯êîÇ…ÇÕ float/double ÇÕéÊÇÍÇ»Ç¢ÇÃÇ≈Ç±Ç§Ç¢Ç§ hack Ç™ïKóv

class CBT601KrKb
{
public:
	static constexpr double KR = COEFF_BT601_KR;
	static constexpr double KB = COEFF_BT601_KB;
};

class CBT709KrKb
{
public:
	static constexpr double KR = COEFF_BT709_KR;
	static constexpr double KB = COEFF_BT709_KB;
};

template<class KRKB>
class CCoefficient
{
public:
	static constexpr double KR    = KRKB::KR;
	static constexpr double KB    = KRKB::KB;

	static constexpr double R2Y   = COEFF_R2Y8(KR, KB);
	static constexpr double G2Y   = COEFF_G2Y8(KR, KB);
	static constexpr double B2Y   = COEFF_B2Y8(KR, KB);
	static constexpr double R2U   = COEFF_R2U8(KR, KB);
	static constexpr double G2U   = COEFF_G2U8(KR, KB);
	static constexpr double B2U   = COEFF_B2U8(KR, KB);
	static constexpr double R2V   = COEFF_R2V8(KR, KB);
	static constexpr double G2V   = COEFF_G2V8(KR, KB);
	static constexpr double B2V   = COEFF_B2V8(KR, KB);

	static constexpr double Y2RGB = COEFF_Y2RGB8;
	static constexpr double U2B   = COEFF_U2B8(KR, KB);
	static constexpr double V2R   = COEFF_V2R8(KR, KB);
	static constexpr double U2G   = COEFF_U2G8(KR, KB);
	static constexpr double V2G   = COEFF_V2G8(KR, KB);
};

typedef CCoefficient<CBT601KrKb> CBT601Coefficient;
typedef CCoefficient<CBT709KrKb> CBT709Coefficient;
