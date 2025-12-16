/* { tts_voicevox.c } : tts with voicevox backend
 */

#include "tts.h"

#pragma comment(lib, "voicevox_core.lib")
#pragma comment(lib, "voicevox_onnxruntime.lib")

error tts_init(void)
    { return error_ok; }

void tts_uninit(void)
    { ; }

error tts_pcmfromutf8(char const* s, struct tts_pcmdesc* ptrd)
{
    (void)s, (void)ptrd;
    return error_ok;
}

void tts_freepcm(struct tts_pcmdesc const d)
    { (void)d; }
