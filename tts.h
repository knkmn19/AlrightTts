/* { tts.h } : pretty much just this one function i tihnk
 */

#ifndef ALRTTS_TTS
#define ALRTTS_TTS

#if (_MSC_VER > 1000)
#   pragma once
#
#endif /* (_MSC_VER > 1000) */

#include "error.h"
#include "types.h"

struct tts_engine {
    unsigned : 0;
    /*
     * rest oif the layout is impl defined
     */
};

struct tts_pcmdesc {
    byte_t* buf;
    size_t sz;
};

error tts_init(void);
void tts_uninit(void);

error tts_createengine(struct tts_engine**);
/*
 * freq     : 48000 hz
 * bitdepth : float32
 * channels : 2
 */
error tts_pcmfromutf8(
    struct tts_engine const*, char const*, struct tts_pcmdesc*
);
void tts_destroyengine(struct tts_engine*);

void tts_freepcm(struct tts_pcmdesc const);

#endif /* !ALRTTS_TTS */
