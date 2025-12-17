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

struct audio_drivermeta {
    void* driver;
    char name[256];
};

struct audio_engine {
    struct audio_drivermeta drivermeta;

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
