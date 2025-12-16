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
    /*
     * set this to true when its false and you want a new track to play
     */
    bool_t bplaying;

    /*
     * dat of track you wanna play
     */
    byte_t* bufread;
    size_t szbufread;

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
