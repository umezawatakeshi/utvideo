/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

class CLogInitializer
{
private:
	CLogInitializer();
	~CLogInitializer();
	static CLogInitializer __li__;

public:
	static void Initialize();
};
