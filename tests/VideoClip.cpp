/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#include "stdafx.h"
#include "VideoClip.h"

VideoClip::VideoClip(string filename) : m_pFormatCtx(NULL)
{
	int err;
	char buf[256];

	if ((err = avformat_open_input(&m_pFormatCtx, ("testclip/" + filename).c_str(), NULL, NULL)) != 0)
	{
		av_strerror(err, buf, sizeof(buf));
		cerr << filename << ": " << buf << endl;
		throw bad_video_clip();
	}

	if ((err = avformat_find_stream_info(m_pFormatCtx, NULL)) < 0)
	{
		av_strerror(err, buf, sizeof(buf));
		cerr << filename << ": " << buf << endl;
		avformat_close_input(&m_pFormatCtx);
		throw bad_video_clip();
	}

	m_nStreamIndex = 0;
	for (; m_nStreamIndex < m_pFormatCtx->nb_streams; ++m_nStreamIndex)
	{
		m_pCodecParam = m_pFormatCtx->streams[m_nStreamIndex]->codecpar;
		if (m_pCodecParam->codec_type == AVMEDIA_TYPE_VIDEO)
			break;
	}

	if (m_nStreamIndex == m_pFormatCtx->nb_streams)
	{
		cerr << filename << ": video stream not found." << endl;
		avformat_close_input(&m_pFormatCtx);
		throw bad_video_clip();
	}
}

VideoClip::~VideoClip()
{
	if (m_pFormatCtx != NULL)
		avformat_close_input(&m_pFormatCtx);
}

DWORD VideoClip::GetFourCC() const
{
	switch (m_pCodecParam->codec_tag)
	{
	case BI_RGB:
		switch (m_pCodecParam->format)
		{
		case AV_PIX_FMT_BGR24:
			return 24;
		case AV_PIX_FMT_BGRA:
			return 32;
		default:
			return 0; // XXX
		}
	default:
		return m_pCodecParam->codec_tag;
	}
}

unsigned int VideoClip::GetWidth() const
{
	return m_pCodecParam->width;
}

unsigned int VideoClip::GetHeight() const
{
	return m_pCodecParam->height;
}

size_t VideoClip::GetExtraData(void *buf, size_t len) const
{
	memcpy(buf, m_pCodecParam->extradata, m_pCodecParam->extradata_size);
	return m_pCodecParam->extradata_size;
}

#define ALLOCATE_ALIGNMENT 4096

int VideoClip::GetNextFrame(void **bufp, size_t *lenp, bool *keyp, size_t alignment)
{
	AVPacket packet;

	ReleaseFrame(bufp);

	while (av_read_frame(m_pFormatCtx, &packet) >= 0)
	{
		if (packet.stream_index == m_nStreamIndex && packet.buf != NULL)
		{
			*bufp = (char*)_aligned_malloc(packet.size + alignment, ALLOCATE_ALIGNMENT) + alignment;
			memcpy(*bufp, packet.data, packet.size);
			*lenp = packet.size;
			if (keyp != NULL)
				*keyp = (packet.flags & AV_PKT_FLAG_KEY);
			av_packet_unref(&packet);
			return 0;
		}

		av_packet_unref(&packet);
	}

	return -1;
}

void VideoClip::ReleaseFrame(void** bufp)
{
	if (*bufp != NULL)
		_aligned_free((void*)((uintptr_t)*bufp & ~(uintptr_t)(ALLOCATE_ALIGNMENT-1)));
	*bufp = NULL;
}

#if 0
int InitFFmpeg()
{
	av_register_all();
	return 0;
}

static int __ffmpeg_init__ = InitFFmpeg();
#endif
