/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#include "stdafx.h"
#include "VideoClip.h"

VideoClip::VideoClip(string filename) : m_pFormatCtx(NULL)
{
	int err;
	char buf[256];

	memset(&m_packet, 0, sizeof(m_packet));

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
	av_packet_unref(&m_packet);

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

int VideoClip::GetNextFrame(void **bufp, size_t *lenp, bool *keyp)
{
	av_packet_unref(&m_packet);

	while (av_read_frame(m_pFormatCtx, &m_packet) >= 0)
	{
		if (m_packet.stream_index == m_nStreamIndex && m_packet.buf != NULL)
		{
			*bufp = m_packet.data;
			*lenp = m_packet.size;
			if (keyp != NULL)
				*keyp = (m_packet.flags & AV_PKT_FLAG_KEY);
			return 0;
		}

		av_packet_unref(&m_packet);
	}

	return -1;
}

int InitFFmpeg()
{
	av_register_all();
	return 0;
}

static int __ffmpeg_init__ = InitFFmpeg();
