/* { tts_stub.c } : tts stub impl
 */

#include "tts.h"

error tts_init(void)
    { return error_fail; }

void tts_destroy(void)
    { ; }

error tts_pcmfromutf8(char const* s, byte_t** ptrm, size_t sz)
{
    (void)s, (void)ptrm, (void)sz;
    return error_fail;
}

void tts_freepcm(byte_t* m)
    { (void)m; }
