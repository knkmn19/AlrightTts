/* { audio_stub.c } : audio stub impl
 */

#include "audio.h"

struct audio_stub_engine {
    struct audio_engine engine;
};

error audio_putdrivermeta(struct audio_drivermeta const** ptrdms)
{
    *ptrdms = NULL;
    return error_ok;
}

void audio_freedrivermeta(struct audio_drivermeta const** ptrdms)
{
    struct audio_drivermeta const* dm = *ptrdms;
    for (;;) {
        if (dm == NULL)
            break;
        (void)dm;
        dm += 1;
    }
}

error audio_createenginewith(
    struct audio_drivermeta const dm, struct audio_engine** ptra
)
{
    (void)dm, (void)ptra;
    return error_fail;
}

void audio_destroyengine(struct audio_engine* a)
    { (void)a; }
