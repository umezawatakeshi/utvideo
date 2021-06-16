/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#pragma once

extern "C"
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4819)
#endif
#include <libavformat/avformat.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

class bad_video_clip
{
};

class VideoClip
{
private:
	AVFormatContext *m_pFormatCtx;
	AVCodecParameters *m_pCodecParam;
	unsigned int m_nStreamIndex;

public:
	VideoClip(string filename);
	~VideoClip();

	DWORD GetFourCC() const;
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	size_t GetExtraData(void *buf, size_t len) const;
	int GetNextFrame(void **bufp, size_t *lenp, bool *keyp, size_t alignment = 0);
	static void ReleaseFrame(void** bufp);
};
