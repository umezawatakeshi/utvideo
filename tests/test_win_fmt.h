/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

struct CODECNAME
{
	wstring wstrShortName;
	wstring wstrLongName;
};

struct CODECRAWFORMAT
{
	DWORD fccCodec;
	DWORD fccRaw;
	typedef DWORD first_type;
	typedef DWORD second_type;
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
		char buf[256]; /* ÉTÉCÉYÇÕìKìñ */
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

template<>
struct print_log_value<CODECRAWFORMAT>
{
	void operator()(ostream& os, const CODECRAWFORMAT& p)
	{
		print_log_value<DWORD> plvdw;
		os << "(";
		plvdw(os, p.fccCodec);
		os << ", ";
		plvdw(os, p.fccRaw);
		os << ")";
	}
};

template<>
struct print_log_value<pair<pair<DWORD, DWORD>, LONG> >
{
	void operator()(ostream& os, const pair<pair<DWORD, DWORD>, LONG>& p)
	{
		print_log_value<DWORD> plvdw;
		print_log_value<LONG> plvl;
		os << "(";
		plvdw(os, p.first.first);
		os << ", ";
		plvdw(os, p.first.second);
		os << ", ";
		plvl(os, p.second);
		os << ")";
	}
};

}}}

extern vector<FOURCC> vecCodecFourcc;
extern vector<CODECNAME> vecCodecName;
extern vector<CODECRAWFORMAT> vecSupportedInputFormatPair;
extern vector<CODECRAWFORMAT> vecSupportedOutputFormatPair;
extern vector<CODECRAWFORMAT> vecUnsupportedInputFormatPair;
extern vector<CODECRAWFORMAT> vecUnsupportedOutputFormatPair;
extern vector<pair<pair<DWORD, DWORD>, LONG> > vecSupportedWidthPair;
extern vector<pair<pair<DWORD, DWORD>, LONG> > vecSupportedHeightPair;
extern vector<pair<pair<DWORD, DWORD>, LONG> > vecUnsupportedWidthPair;
extern vector<pair<pair<DWORD, DWORD>, LONG> > vecUnsupportedHeightPair;

static inline bool IsFourccRGB(DWORD dw)
{
	return dw <= 0xff || dw >= (DWORD)-0xff;
}

static inline DWORD FCC2Compression(DWORD dw)
{
	if (IsFourccRGB(dw))
		return BI_RGB;
	else

		return dw;
}

static inline WORD FCC2BitCount(DWORD dw)
{
	if (IsFourccRGB(dw))
		return (WORD)abs((int)dw);

	switch (dw)
	{
	case FCC('YUY2'):
	case FCC('YUYV'):
	case FCC('YUNV'):
	case FCC('UYVY'):
	case FCC('UYNV'):
	case FCC('yuvs'):
	case FCC('2vuy'):
	case FCC('HDYC'):
		return 16;
	case FCC('YV12'):
		return 12;
	case FCC('v210'):
		return 20;
	default:
		return 0; // XXX
	}
}

static inline int FCC2HeightSign(DWORD dw)
{
	if (IsFourccRGB(dw))
	{
		if ((int)dw > 0)
			return 1;
		else if ((int)dw < 0)
			return -1;
		else
			return 0;
	}
	else
		return 1;
}
