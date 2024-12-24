#ifndef _GBS_AUDIO_CODEC_H_
#define _GBS_AUDIO_CODEC_H_

#include <cstdint>

enum FrameFormat {
	FRAME_FORMAT_UNKNOWN = 0,

	FRAME_FORMAT_I420 = 100,

	FRAME_FORMAT_VP8 = 200,
	FRAME_FORMAT_VP9,
	FRAME_FORMAT_H264,
	FRAME_FORMAT_H265,
	FRAME_FORMAT_AV1,

	FRAME_FORMAT_MSDK = 300,

	FRAME_FORMAT_PCM_48000_2 = 800,

	FRAME_FORMAT_PCMU = 900,
	FRAME_FORMAT_PCMA,
	FRAME_FORMAT_OPUS,
	FRAME_FORMAT_ISAC16,
	FRAME_FORMAT_ISAC32,
	FRAME_FORMAT_ILBC,
	FRAME_FORMAT_G722_16000_1,
	FRAME_FORMAT_G722_16000_2,

	FRAME_FORMAT_AAC,         // ignore sample rate and channels for decoder, default is 48000_2
	FRAME_FORMAT_AAC_48000_2, // specify sample rate and channels for encoder

	FRAME_FORMAT_AC3,
	FRAME_FORMAT_NELLYMOSER,

	FRAME_FORMAT_DATA, // Generic data frame. We don't know its detailed structure.

	FRAME_FORMAT_RTP, // RTP packet.
};

struct AudioFrame {
	void *data;
	int sample_rate_hz;
	int num_channels;
	size_t samples_per_channel;
	uint64_t ts;
};

struct Frame {
	enum Format { FRAME_FORMAT_OPUS };

	Format format;
	uint8_t *payload;
	size_t length;
	struct {
		struct {
			int nbSamples;
			int sampleRate;
			int channels;
		} audio;
	} additionalInfo;
	int64_t timeStamp;
};


class GBSAudioEncoder
{
public:
	// Must have virtual destructor to ensure child class's destructor is called
	virtual ~GBSAudioEncoder() {};
	virtual int Encode(short *in, int inLen, unsigned char *out, int outLen) = 0;
	virtual int EncodeF(const float *in, int inLen, unsigned char *out, int outLen) = 0;
	virtual int TrySetRate(int rate) = 0;
	virtual int GetRate() = 0;
	virtual int GetClockRate() = 0;
	int			numFrameSamples;
	int			frameLength;
};

class GBSAudioDecoder
{
public:
	// Must have virtual destructor to ensure child class's destructor is called
	virtual ~GBSAudioDecoder() {};
	virtual int Decode(unsigned char *in, int inLen, short *out, int outLen) = 0;
	virtual int TrySetRate(int rate) = 0;
	virtual int GetRate() = 0;
	int			numFrameSamples;
	int			frameLength;
};


#endif //_GBS_AUDIO_CODEC_H_
