// Copyright (C) <2019> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef GBS_OPUS_ENCODER_H
#define GBS_OPUS_ENCODER_H

#include <memory>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/audio_fifo.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
#include <libavutil/error.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libavutil/mem.h>
#include <libavutil/time.h>
#include <libavutil/fifo.h>
#include <libavutil/audio_fifo.h>
}
#include "GBSAudioCodec.h"


class GBSOpusEncoder {
public:
    GBSOpusEncoder();
    virtual ~GBSOpusEncoder();

    bool init();
    bool addAudioFrame(const AudioFrame* audioFrame);

private:
    bool initEncoder();
    bool addToFifo(const AudioFrame* audioFrame);
    void encode();
    void sendOut(AVPacket& pkt);
    char* ff_err2str(int errRet);

    bool m_valid;
    int m_channels;
    int m_sampleRate;
    AVCodecContext* m_audioEnc;
    AVAudioFifo* m_audioFifo;
    AVFrame* m_audioFrame;
    char m_errbuff[500];
    AVFormatContext* m_formatCtx;
    AVStream* m_stream;

};


#endif // GBS_OPUS_ENCODER_H
