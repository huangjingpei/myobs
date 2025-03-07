#include <obs-module.h>
#include <util/threading.h>
#include <util/platform.h>
#include <util/dstr.h>
#include <sys/stat.h>

#define blog(log_level, format, ...) \
	blog(log_level, "[rtc_source: '%s'] " format, obs_source_get_name(context->source), ##__VA_ARGS__)

#define debug(format, ...) blog(LOG_DEBUG, format, ##__VA_ARGS__)
#define info(format, ...) blog(LOG_INFO, format, ##__VA_ARGS__)
#define warn(format, ...) blog(LOG_WARNING, format, ##__VA_ARGS__)

#define BUFFER_SIZE_MAX (16000)
#define DEFAULT_AUDIO_MIX_IDX 0
#define AUDIO_SAMPLE_TYPE float  // Use float as the sample type
#define AUDIO_OUTPUT_FRAMES 1024 // Example: Frames per audio output (tune as needed)

struct pcm_source {
	obs_source_t *source;
	uint32_t sample_rate;
	uint32_t channels;
	enum audio_format format;
	uint8_t *buffer; // Raw byte buffer
	size_t buffer_size;
	uint8_t *ring_buffer;
	size_t ring_buffer_size;
	size_t ring_buffer_read_pos;
	size_t ring_buffer_write_pos;

	float *float_buffer[MAX_AUDIO_CHANNELS]; // Float buffer for OBS per channel
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	bool active;
};
static inline size_t audio_format_get_bytes_per_sample(enum audio_format format)
{
	switch (format) {
	case AUDIO_FORMAT_U8BIT:
		return 1;
	case AUDIO_FORMAT_16BIT:
		return 2; // 16-bit PCM
	case AUDIO_FORMAT_32BIT:
		return 4; // 32-bit PCM
	case AUDIO_FORMAT_FLOAT:
		return 4;
	case AUDIO_FORMAT_U8BIT_PLANAR:
	case AUDIO_FORMAT_16BIT_PLANAR:
	case AUDIO_FORMAT_32BIT_PLANAR:
	case AUDIO_FORMAT_FLOAT_PLANAR:
		return 0; //Planar type is not handle at here
	default:
		return 0; // Or handle error
	}
}
static const char *pcm_source_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return "PCM Audio Source";
}

static void *pcm_source_create(obs_data_t *settings, obs_source_t *source)
{
	struct pcm_source *context = bzalloc(sizeof(struct pcm_source));
	context->source = source;
	context->sample_rate = 44100;
	context->channels = 2;
	context->format = AUDIO_FORMAT_16BIT;
	context->ring_buffer_size = BUFFER_SIZE_MAX * 4;
	context->ring_buffer = bzalloc(context->ring_buffer_size);
	context->ring_buffer_read_pos = 0;
	context->ring_buffer_write_pos = 0;

	context->buffer = bzalloc(BUFFER_SIZE_MAX);
	context->buffer_size = 0;

	// Allocate float buffers for each channel
	for (int i = 0; i < MAX_AUDIO_CHANNELS; ++i) {
		context->float_buffer[i] = bzalloc(AUDIO_OUTPUT_FRAMES * sizeof(float));
	}

	pthread_mutex_init(&context->mutex, NULL);
	pthread_cond_init(&context->cond, NULL);
	context->active = false;
	return context;
}

static void pcm_source_destroy(void *data)
{
	struct pcm_source *context = data;
	if (context) {
		context->active = false;
		pthread_cond_signal(&context->cond);
		pthread_mutex_destroy(&context->mutex);
		pthread_cond_destroy(&context->cond);
		bfree(context->ring_buffer);
		bfree(context->buffer);

		// Free float buffers for each channel
		for (int i = 0; i < MAX_AUDIO_CHANNELS; ++i) {
			bfree(context->float_buffer[i]);
		}
		bfree(context);
	}
}

static obs_properties_t *pcm_source_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();

	obs_properties_add_int(props, "sample_rate", "Sample Rate", 8000, 192000, 1000);
	obs_properties_add_int(props, "channels", "Channels", 1, 2, 1);

	obs_property_t *p =
		obs_properties_add_list(props, "format", "Format", OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);

	obs_property_list_add_int(p, "16-bit PCM", AUDIO_FORMAT_16BIT);
	obs_property_list_add_int(p, "32-bit PCM", AUDIO_FORMAT_32BIT);

	return props;
}

static void pcm_source_defaults(obs_data_t *settings)
{
	obs_data_set_default_int(settings, "sample_rate", 44100);
	obs_data_set_default_int(settings, "channels", 2);
	obs_data_set_default_int(settings, "format", AUDIO_FORMAT_16BIT);
}

static void pcm_source_update(void *data, obs_data_t *settings)
{
	struct pcm_source *context = data;
	pthread_mutex_lock(&context->mutex);
	context->sample_rate = (uint32_t)obs_data_get_int(settings, "sample_rate");
	context->channels = (uint32_t)obs_data_get_int(settings, "channels");
	context->format = (enum audio_format)obs_data_get_int(settings, "format");
	context->ring_buffer_read_pos = 0;
	context->ring_buffer_write_pos = 0;
	pthread_mutex_unlock(&context->mutex);
}

void push_pcm_data(void *data, const uint8_t *pcm_data, size_t size)
{
	struct pcm_source *context = data;

	if (!context || !pcm_data || size == 0)
		return;

	pthread_mutex_lock(&context->mutex);

	while (context->active &&
	       (context->ring_buffer_write_pos + size) > (context->ring_buffer_read_pos + context->ring_buffer_size)) {
		pthread_cond_wait(&context->cond, &context->mutex);
	}
	if (!context->active) {
		pthread_mutex_unlock(&context->mutex);
		return;
	}

	if (context->ring_buffer_write_pos + size > context->ring_buffer_size) {
		size_t remaining = context->ring_buffer_size - context->ring_buffer_write_pos;
		memcpy(context->ring_buffer + context->ring_buffer_write_pos, pcm_data, remaining);
		memcpy(context->ring_buffer, pcm_data + remaining, size - remaining);
		context->ring_buffer_write_pos = size - remaining;
	} else {
		memcpy(context->ring_buffer + context->ring_buffer_write_pos, pcm_data, size);
		context->ring_buffer_write_pos += size;
	}

	if (context->ring_buffer_write_pos >= context->ring_buffer_size) {
		context->ring_buffer_write_pos -= context->ring_buffer_size;
	}

	pthread_cond_signal(&context->cond);
	pthread_mutex_unlock(&context->mutex);
}


static inline bool pcm_audio_render(void *data, uint64_t *ts_out, struct obs_source_audio_mix *audio_output,
					    uint32_t mixers, size_t channels,
	 size_t sample_rate)
{
	struct pcm_source *context = (struct pcm_source *)(data);

	if (!context->active) {
		return false;
	}

	pthread_mutex_lock(&context->mutex);

	// Check the amount of available data in the ring buffer.
	size_t available_data =
		(context->ring_buffer_write_pos + context->ring_buffer_size - context->ring_buffer_read_pos) %
		context->ring_buffer_size;
	size_t needed_bytes = AUDIO_OUTPUT_FRAMES * channels * sizeof(float); // Bytes needed per frame for all channels

	if (available_data < needed_bytes) {
		pthread_mutex_unlock(&context->mutex);
		return false;
	}

	// Read from ring buffer
	size_t read_bytes = needed_bytes > BUFFER_SIZE_MAX ? BUFFER_SIZE_MAX : needed_bytes; // Limit to BUFFER_SIZE_MAX
	if (context->ring_buffer_read_pos + read_bytes > context->ring_buffer_size) {
		size_t remaining = context->ring_buffer_size - context->ring_buffer_read_pos;
		memcpy(context->buffer, context->ring_buffer + context->ring_buffer_read_pos, remaining);
		memcpy(context->buffer + remaining, context->ring_buffer, read_bytes - remaining);
		context->ring_buffer_read_pos = read_bytes - remaining;
	} else {
		memcpy(context->buffer, context->ring_buffer + context->ring_buffer_read_pos, read_bytes);
		context->ring_buffer_read_pos += read_bytes;
	}
	if (context->ring_buffer_read_pos >= context->ring_buffer_size) {
		context->ring_buffer_read_pos -= context->ring_buffer_size;
	}

	// Convert the audio data to float format for OBS and copy to channel buffers
	int num_frames = (int)(read_bytes / (channels * audio_format_get_bytes_per_sample(
							  context->format))); // Num of frames to conver per channel

	const void *source_data = context->buffer; // Use bytes read from buffer
	for (size_t ch = 0; ch < channels; ch++) {
		float *out = audio_output->output[DEFAULT_AUDIO_MIX_IDX].data[ch]; // get channel
		if (context->format == AUDIO_FORMAT_16BIT) {
			int16_t *in = (int16_t *)source_data + ch; // start from correct channel

			for (size_t i = 0; i < num_frames; ++i) {
				out[i] = (float)in[i * channels] / INT16_MAX; // convert from int16
			}
		} else if (context->format == AUDIO_FORMAT_32BIT) {
			int32_t *in = (int32_t *)source_data + ch; // start from correct channel
			for (size_t i = 0; i < num_frames; ++i) {
				out[i] = (float)in[i * channels] / INT32_MAX; // convert from int32
			}
		}
	}

	//Assign all channel
	for (size_t mix = 0; mix < MAX_AUDIO_MIXES; mix++) {
		if ((mixers & (1 << mix)) == 0)
			continue;

		for (size_t ch = 0; ch < channels; ch++) {
			audio_output->output[mix].data[ch] = context->float_buffer[ch];
		}
	}

	pthread_cond_signal(&context->cond);
	pthread_mutex_unlock(&context->mutex);
	*ts_out = os_gettime_ns();
	return true;
}

static void pcm_activate(void *data)
{
	struct pcm_source *context = data;
	pthread_mutex_lock(&context->mutex);
	context->active = true;
	pthread_mutex_unlock(&context->mutex);
	info("Activated PCM Source");
}

static void pcm_deactivate(void *data)
{
	struct pcm_source *context = data;
	pthread_mutex_lock(&context->mutex);
	context->active = false;
	pthread_mutex_unlock(&context->mutex);
	info("Deactivated PCM Source");
}

OBS_DECLARE_MODULE()
MODULE_EXPORT const char *obs_module_description(void)
{
	return "rtc audio pcm source ";
}

struct obs_source_info pcm_source = {
	.id = "pcm_audio_source",
	.type = OBS_SOURCE_TYPE_INPUT,
	.output_flags = OBS_SOURCE_AUDIO,
	.get_name = pcm_source_get_name,
	.create = pcm_source_create,
	.destroy = pcm_source_destroy,
	.activate = pcm_activate,
	.deactivate = pcm_deactivate,
	.audio_render = pcm_audio_render,
	.get_properties = pcm_source_properties,
	.get_defaults = pcm_source_defaults,
	.update = pcm_source_update,

};

bool obs_module_load(void)
{
	obs_register_source(&pcm_source);
	return true;
}
