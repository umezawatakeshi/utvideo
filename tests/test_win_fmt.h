/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

namespace boost { namespace test_tools { namespace tt_detail {

template<>
struct print_log_value<DWORD>
{
	void operator()(ostream& os, const DWORD& fcc)
	{
		union
		{
			DWORD dw;
			char buf[16];
		};
		dw = fcc;

		if (isprint(buf[0]) && isprint(buf[1]) && isprint(buf[2]) && isprint(buf[3]))
			buf[4] = '\0';
		else
			sprintf(buf, "0x%08x", dw);

		os << buf;
	}
};

}}}

extern vector<FOURCC> vecCodecFourcc;
