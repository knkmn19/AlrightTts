/* { tts_voicevox.c } : tts with voicevox backend
 */

#include "tts.h"

#pragma comment(lib, "voicevox_core.lib")
#pragma comment(lib, "voicevox_onnxruntime.lib")

#include "voicevox/voicevox_core.h"

static error error_errorfromvoicevox(VoicevoxResultCode);

static error error_errorfromvoicevox(VoicevoxResultCode r)
{
    switch (r) {
    default:
        break;

    case VOICEVOX_RESULT_OK:
        return error_ok;
    }

    return error_fail;
}

error tts_init(void)
{
    VoicevoxResultCode r;

    struct VoicevoxOnnxruntime const* onnxrt;
    r = voicevox_onnxruntime_load_once(
        voicevox_make_default_load_onnxruntime_options(), &onnxrt
    );
    if (r != VOICEVOX_RESULT_OK)
        return error_errorfromvoicevox(r);

    return error_ok;
}

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
