/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

class CBGRColorOrder
{
public:
	static constexpr int A = INT_MIN;
	static constexpr int R = 2;
	static constexpr int G = 1;
	static constexpr int B = 0;

	static constexpr bool HAS_ALPHA = false;
	static constexpr int BYPP = 3;
};

class CBGRAColorOrder
{
public:
	static constexpr int A = 3;
	static constexpr int R = 2;
	static constexpr int G = 1;
	static constexpr int B = 0;

	static constexpr bool HAS_ALPHA = true;
	static constexpr int BYPP = 4;
};

class CRGBColorOrder
{
public:
	static constexpr int A = INT_MIN;
	static constexpr int R = 0;
	static constexpr int G = 1;
	static constexpr int B = 2;

	static constexpr bool HAS_ALPHA = false;
	static constexpr int BYPP = 3;
};

class CARGBColorOrder
{
public:
	static constexpr int A = 0;
	static constexpr int R = 1;
	static constexpr int G = 2;
	static constexpr int B = 3;

	static constexpr bool HAS_ALPHA = true;
	static constexpr int BYPP = 4;
};

class CYUYVColorOrder
{
public:
	static constexpr int Y0 = 0;
	static constexpr int Y1 = 2;
	static constexpr int U  = 1;
	static constexpr int V  = 3;
};

class CUYVYColorOrder
{
public:
	static constexpr int Y0 = 1;
	static constexpr int Y1 = 3;
	static constexpr int U  = 0;
	static constexpr int V  = 2;
};

class CB64aColorOrder
{
public:
	static constexpr int A = 0;
	static constexpr int R = 1;
	static constexpr int G = 2;
	static constexpr int B = 3;

	static constexpr bool HAS_ALPHA = true;
	static constexpr int BYPP = 8;
};

class CB48rColorOrder
{
public:
	static constexpr int A = INT_MIN;
	static constexpr int R = 0;
	static constexpr int G = 1;
	static constexpr int B = 2;

	static constexpr bool HAS_ALPHA = false;
	static constexpr int BYPP = 6;
};
