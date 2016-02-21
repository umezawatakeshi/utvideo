/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

struct CODECNAME
{
	wstring wstrShortName;
	wstring wstrLongName;
};

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

		if (isprint((unsigned char)buf[0]) && isprint((unsigned char)buf[1]) && isprint((unsigned char)buf[2]) && isprint((unsigned char)buf[3]))
			buf[4] = '\0';
		else
			sprintf(buf, "0x%08x", dw);

		os << buf;
	}
};

template<>
struct print_log_value<wstring>
{
	void operator()(ostream &os, const wstring &wstr)
	{
		char buf[256]; /* ƒTƒCƒY‚Í“K“– */
		wcstombs(buf, wstr.c_str(), sizeof(buf));

		os << "\"" << buf << "\"";
	}
};

template<>
struct print_log_value<CODECNAME>
{
	void operator()(ostream& os, const CODECNAME& p)
	{
		print_log_value<wstring> plvwstr;
		os << "(";
		plvwstr(os, p.wstrShortName);
		os << ", ";
		plvwstr(os, p.wstrLongName);
		os << ")";
	}
};

}}}

extern vector<FOURCC> vecCodecFourcc;
extern vector<CODECNAME> vecCodecName;
