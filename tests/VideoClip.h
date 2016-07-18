/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

extern "C"
{
#include <libavformat/avformat.h>
}

class bad_video_clip
{
};

class VideoClip
{
private:
	AVFormatContext *m_pFormatCtx;
	AVCodecParameters *m_pCodecParam;
	AVPacket m_packet;
	unsigned int m_nStreamIndex;

public:
	VideoClip(string filename);
	~VideoClip();

	DWORD GetFourCC() const;
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	size_t GetExtraData(void *buf, size_t len) const;
	int GetNextFrame(void **bufp, size_t *lenp, bool *keyp);
};
