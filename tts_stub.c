/* { tts_stub.c } : tts stub impl
 */

#include "tts.h"

error tts_init(void)
    { return error_ok; }

void tts_uninit(void)
    { ; }

error tts_createengine(struct tts_engine** ptre)
{
    (void)ptre;
    return error_ok;
}

error tts_pcmfromutf8(
    struct tts_engine* const e, char const* s, struct tts_pcmdesc* ptrd
)
{
    (void)e, (void)s, (void)ptrd;
    return error_ok;
}

void tts_destroyengine(struct tts_engine* e)
    { (void)e; }

void tts_freepcm(struct tts_pcmdesc const d)
    { (void)d; }
