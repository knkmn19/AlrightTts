// AlrightTts.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

extern "C" {
    #include "audio.h"
    #include "tts.h"
    #include "system.h"
} // extern "C"

#include <iostream>
#include <string>
#include "scopeexit.hpp"

inline static error printerror(char const* where, error e)
{
    std::cerr << where << " : " << ::error_what(e) << std::endl;
    return e;
}

/*
 * init audio and tts subsystems
 */
static int init(void);

/*
 * enumerate drivers
 * ask for which one(s
 * create audio engine
 */
static int setupaudio(audio_engine*&);

/*
 * not much customisation here but
 */
static int setuptts(tts_engine*&);

/*
 * wait for input ending with \n
 * write each character from stdin into variable length string
 * convert string to wav
 * if pcm already inflight in audio wait
 * set pcm in audio
 * continue
 */
static int startkernel(audio_engine*);
static std::string strfrominbuf(void);

/*
 * clean up
 */
static void uninit(void);

static int init(void)
{
    error e;
    if (e = ::audio_init())
        return ::printerror("init", e);

    if (e = ::tts_init())
        return ::printerror("init", e);

    if (e = ::console_init())
        return ::printerror("init", e);

    return 0;
}

static int setupaudio(audio_engine*& ptra)
{
    error e;

    audio_engine* o;

    audio_drivermeta const* metas;
    size_t noMeta;
    if (e = ::audio_putdrivermeta(&metas, &noMeta))
        return ::printerror("audio", e);

    auto seDm = ScopeExit(
        [&metas](void) -> void
            { ::audio_freedrivermeta(metas); }
    );

    std::cout << "pick one from" << std::endl;
    for (size_t i = 0; i < noMeta; i++) {
        audio_drivermeta const& dm = metas[i];
        std::cout << "[" << i << "] - " << dm.name << std::endl;
    }

    size_t iSelected = noMeta;
    while (iSelected >= noMeta)
        iSelected = (std::getchar() - '0');

    if (e = ::audio_createenginewith(metas[iSelected], &o))
        return ::printerror("audio", e);

    ptra = o;
    return 0;
}

static int setuptts(tts_engine*& ptre)
{
    error e;
    if (e = ::tts_createengine(&ptre))
        return ::printerror("tts", e);

    return 0;
}

static std::string strfrominbuf(void)
{
    std::string in;
    in.clear();

    for (;;) {
        byte_t c;
        while ((c = std::getchar()) != '\n')
            in += c;

        if (in.length() > 0u)
            break;
    }

    return in;
}

static int startkernel(audio_engine* a, tts_engine* tts)
{
    error e;

    tts_pcmdesc dPrev = { };
    dPrev.buf = nullptr;
    for (;;) {
        bool bSigint = false;
        if (bSigint)
            break;

        std::string in = ::strfrominbuf();

        tts_pcmdesc d = { };
        if (e = ::tts_pcmfromutf8(tts, in.c_str(), &d))
            return ::printerror("kernel", e);

        /*
         * its aligned but should still mmake it clear this is an atomic loadd
         */
        while (a->bplaying)
            ;

        if (dPrev.buf != nullptr)
            ::tts_freepcm(dPrev);

        a->pcmdesc = d;
        a->bplaying = true;

        dPrev = d;
    }

    return 0;
}

static void uninit(void)
{
    ::tts_uninit();
    ::audio_uninit();
}

int main(int, char** vector)
{
    int o;

    (void)vector;

    if (o = ::init())
        return o;

    auto se = ScopeExit(
        [](void) -> void
            { ::uninit(); }
    );

    audio_engine* a;
    tts_engine* tts;
    if (o = ::setupaudio(a))
        return o;

    auto seAudio = ScopeExit(
        [&a](void) -> void
            { ::audio_destroyengine(a); }
    );

    if (o = ::setuptts(tts))
        return o;

    auto seTts = ScopeExit(
        [&tts](void) -> void
            { ::tts_destroyengine(tts); }
    );

    std::cout << "ready!!!!!!" << std::endl;
    if (o = ::startkernel(a, tts))
        return o;

    std::cout << "Hello World!\n";

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
