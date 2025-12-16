/* { tts_stub.c } : tts stub impl
 */

#include "tts.h"

error tts_init(void)
    { return error_ok; }

void tts_destroy(void)
    { ; }

error tts_pcmfromutf8(char const* s, struct tts_pcmdesc* ptrd)
{
    (void)s, (void)ptrd;
    return error_ok;
}

void tts_freepcm(struct tts_pcmdesc const d)
    { (void)d; }
