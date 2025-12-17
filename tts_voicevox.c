/* { tts_voicevox.c } : tts with voicevox backend
 */

#include "tts.h"

#pragma comment(lib, "voicevox_core.lib")
#pragma comment(lib, "voicevox_onnxruntime.lib")

#include "voicevox/voicevox_core.h"

#include <stdlib.h>
#include <string.h>
#include "debug.h"

struct tts_voicevox_engine {
    struct tts_engine engine;

    struct VoicevoxSynthesizer* synthesizer;
};

#pragma pack(1)
struct wav_chunk_fmt {
    byte_t      magicfmt[4];
    dword_t     szchunk;
    word_t      format;
    word_t      nochannels;
    dword_t     freq;
    dword_t     : 32;
    word_t      : 16;
    word_t      bits;
};
#pragma pack(pop)

static error error_errorfromvoicevox(VoicevoxResultCode);

static error tts_voicevox_engine_setupsynthesizer(struct tts_voicevox_engine*);

static error tts_createquery(
    struct VoicevoxSynthesizer*, char const* text, char** ptrq
);

static error tts_pcmfromquery(
    char const* q, struct VoicevoxSynthesizer* synth, struct tts_pcmdesc*
);

static error tts_pcmfromvvwav(
    struct tts_pcmdesc const wav, struct tts_pcmdesc* pcm
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

static error tts_pcmfromquery(
    char const* q, struct VoicevoxSynthesizer* synth, struct tts_pcmdesc* ptrd
)
{
    error e;
    VoicevoxResultCode r;

    VoicevoxStyleId const styleid = 14;

    struct tts_pcmdesc wav;
    r = voicevox_synthesizer_synthesis(
        synth, q, styleid, voicevox_make_default_synthesis_options(),
        &wav.sz, &wav.buf
    );
    if (r != VOICEVOX_RESULT_OK)
        return error_errorfromvoicevox(r);

    e = tts_pcmfromvvwav(wav, ptrd);
    if (e != error_ok)
        return e;

    return error_fail;
}

static error tts_pcmfromvvwav(
    struct tts_pcmdesc const wav, struct tts_pcmdesc* ptrd
)
{
    struct tts_pcmdesc o;

    byte_t* datwav = wav.buf;
    while (*(dword_t*)datwav != *(dword_t*)"fmt ")
        datwav += 1;

    struct wav_chunk_fmt* chunkfmt = datwav;
    assert(chunkfmt->freq == 24000);
    assert(chunkfmt->format == 1);
    assert(chunkfmt->bits == 16);
    assert(chunkfmt->nochannels == 1);

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
    struct tts_engine* const en, char const* s, struct tts_pcmdesc* ptrd
)
{
    error e = error_ok;
    struct tts_voicevox_engine* engine = (struct tts_voicevox_engine*)en;

    char* q;
    e = tts_createquery(engine->synthesizer, s, &q);
    if (e != error_ok)
        goto ret;

    e = tts_pcmfromquery(q, engine->synthesizer, ptrd);
    if (e != error_ok)
        goto freequery;

freequery:
    voicevox_json_free(q);

ret:
    return e;
}

void tts_destroyengine(struct tts_engine* e)
    { (void)e; }

void tts_freepcm(struct tts_pcmdesc const d)
    { (void)d; }
