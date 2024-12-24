#ifndef __PCM_SOURCE_H__
#define __PCM_SOURCE_H__
#include <stdint.h>
#if defined(__cplusplus)
extern "C" {
#endif

void push_pcm_data(void *data, const uint8_t *pcm_data, size_t size);
#if defined(__cplusplus)
}
#endif
#endif //__PCM_SOURCE_H__
