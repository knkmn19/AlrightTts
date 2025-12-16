/* { audio_stub.c } : audio stub impl
 */

#include "audio.h"

struct audio_stub_engine {
    struct audio_engine engine;
};

error audio_init(void)
    { return error_ok; }

void audio_destroy(void)
    { ; }

error audio_putdrivermeta(struct audio_drivermeta const** ptrdms, size_t* no)
{
    *ptrdms = NULL;
    *no = 0u;
    return error_ok;
}

void audio_freedrivermeta(struct audio_drivermeta const* dms)
    { (void)dms; }

error audio_createenginewith(
    struct audio_drivermeta const dm, struct audio_engine** ptra
)
{
    (void)dm, (void)ptra;
    return error_fail;
}

void audio_destroyengine(struct audio_engine* a)
    { (void)a; }
