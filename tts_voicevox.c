/* { tts_voicevox.c } : tts with voicevox backend
 */

#include "tts.h"

#pragma comment(lib, "voicevox_core.lib")
#pragma comment(lib, "voicevox_onnxruntime.lib")

#include <stdlib.h>
#include <string.h>
#include "voicevox/voicevox_core.h"

struct tts_voicevox_engine {
    struct tts_engine engine;

    struct VoicevoxSynthesizer* synthesizer;
};

static error error_errorfromvoicevox(VoicevoxResultCode);

static error tts_voicevox_engine_setupsynthesizer(struct tts_voicevox_engine*);

static error tts_createquery(
    struct VoicevoxSynthesizer*, char const* text, char** ptrq
);

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

static error tts_voicevox_engine_setupsynthesizer(
    struct tts_voicevox_engine* e
)
{
    VoicevoxResultCode r;
    struct VoicevoxSynthesizer** ptrsynthesizer = &e->synthesizer;

    /*
     * gonna hard code all the paths for now cause
     */
    struct VoicevoxVoiceModelFile* model;
    struct OpenJtalkRc* ojt;

    /*
     * | VVMファイル名 | 話者名 | スタイル名 | スタイルID |
     * | 1.vvm | 冥鳴ひまり | ノーマル | 14 |
     */
    r = voicevox_voice_model_file_open("./models/vvms/1.vvm", &model);
    if (r != VOICEVOX_RESULT_OK)
        goto ret;

    r = voicevox_open_jtalk_rc_new("./open_jtalk_dic_utf_8-1.11", &ojt);
    if (r != VOICEVOX_RESULT_OK)
        goto deletemodel;

    r = voicevox_synthesizer_new(
        voicevox_onnxruntime_get(), ojt,
        voicevox_make_default_initialize_options(),
        ptrsynthesizer
    );
    if (r != VOICEVOX_RESULT_OK)
        goto deleteojt;

    r = voicevox_synthesizer_load_voice_model(*ptrsynthesizer, model);
    if (r != VOICEVOX_RESULT_OK)
        goto deleteojt;

deleteojt:
    voicevox_open_jtalk_rc_delete(ojt);

deletemodel:
    voicevox_voice_model_file_delete(model);

ret:
    return error_errorfromvoicevox(r);
}

static error tts_createquery(
    struct VoicevoxSynthesizer* synth, char const* text, char** ptrq
)
{
    VoicevoxResultCode r;
    char* o;

    VoicevoxStyleId const styleid = 14;
    r = voicevox_synthesizer_create_audio_query(synth, text, styleid, &o);
    if (r != VOICEVOX_RESULT_OK)
        return error_errorfromvoicevox(r);

    *ptrq = o;
    return error_ok;
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
    error e;
    struct tts_voicevox_engine* o;

    o = malloc(sizeof *o);
    if (o == NULL)
        return error_badalloc;
    memset(o, 0x00, sizeof *o);

    e = tts_voicevox_engine_setupsynthesizer(o);
    if (e != error_ok)
        goto freeengine;

    *ptre = o;
    return error_ok;

freeengine:
    free(o);

    return e;
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
