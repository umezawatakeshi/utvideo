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
struct print_log_value<GUID>
{
	void operator()(ostream& os, const GUID& guid)
	{
		RPC_CSTR psz;

		UuidToString(&guid, &psz);
		os << (char *)psz;
		RpcStringFree(&psz);
	}
};

template<typename T>
struct print_log_value<vector<T>>
{
	void operator()(ostream& os, const vector<T>& vec)
	{
		print_log_value<T> ple;

		if (vec.empty())
		{
			os << "{}";
		}
		else
		{
			os << "{ ";
			ple(os, vec.front());
			for (auto it = vec.begin() + 1, end = vec.end(); it != end; ++it)
			{
				os << ", ";
				ple(os, *it);
			}
			os << " }";
		}
	}
};

}}}
