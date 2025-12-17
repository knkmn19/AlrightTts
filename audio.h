/* { audio.h } : audio output thing
 */

#ifndef ALRTTS_AUDIO
#define ALRTTS_AUDIO

#if (_MSC_VER > 1000)
#   pragma once
#
#endif /* (_MSC_VER > 1000) */

#include "types.h"
#include "error.h"
#include "tts.h"

typedef enum {
    audio_format_u8,
    audio_format_i16,
    audio_format_f32,
} audio_format;

struct audio_drivermeta {
    void* driver;
    char name[256];
};

struct audio_meta {
    u8_t nochannels;
    u32_t hzfreq;
    u32_t bitssample;
    audio_format format;
};

struct audio_engine {
    struct audio_drivermeta drivermeta;
    struct audio_meta meta;

    bool_t bplaying;
    struct tts_pcmdesc pcmdesc;
    /*
     * rest is variable length
     */
};

error audio_init(void);
void audio_uninit(void);

error audio_putdrivermeta(struct audio_drivermeta const**, size_t* no);
void audio_freedrivermeta(struct audio_drivermeta const*);

error audio_createenginewith(
    struct audio_drivermeta const, struct audio_engine**
);
void audio_destroyengine(struct audio_engine*);

#endif /* !ALRTTS_AUDIO */
