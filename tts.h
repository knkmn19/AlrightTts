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

error tts_init(void);
void tts_destroy(void);

error tts_pcmfromutf8(char const*, byte_t**, size_t);
void tts_freepcm(byte_t*);

#endif /* !ALRTTS_TTS */
