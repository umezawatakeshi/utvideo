/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */
/* $Id$ */

#pragma once

class ICCloser
{
private:
	HIC m_hic;

public:
	ICCloser(HIC hic) : m_hic(hic)
	{
	}

	~ICCloser()
	{
		ICClose(m_hic);
	}
};