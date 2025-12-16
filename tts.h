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

typedef enum {
    tts_pcmformat_int16,
} tts_pcmformat;

struct tts_pcmdesc {
    byte_t* buf;
    size_t sz;
    tts_pcmformat format;
};

error tts_init(void);
void tts_destroy(void);

error tts_pcmfromutf8(char const*, struct tts_pcmdesc*);
void tts_freepcm(struct tts_pcmdesc const);

#endif /* !ALRTTS_TTS */
