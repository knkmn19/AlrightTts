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
