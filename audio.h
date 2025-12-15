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

struct audio_drivermeta {
    void* driver;
    char name[256];
};

struct audio_engine {
    byte_t* bufread;
    size_t szbufread;
    /*
     * rest is variable length
     */
};

/*
 * NULL sentinel
 */
error audio_putdrivermeta(struct audio_drivermeta const**);
error audio_freedrivermeta(struct audio_drivermeta const**);

error audio_createenginewith(
    struct audio_drivermeta const, struct audio_engine**
);
error audio_destroyengine(struct audio_engine*);

#endif /* !ALRTTS_AUDIO */
