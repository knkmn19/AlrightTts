// AlrightTts.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

extern "C" {
    #include "audio.h"
    #include "tts.h"
} // extern "C"

#include <iostream>
#include "scopeexit.hpp"

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
 * wait for input ending with \n
 * write each character from stdin into variable length string
 * convert string to wav
 * if pcm already inflight in audio wait
 * set pcm in audio
 * continue
 */
static int startkernel(void);

/*
 * clean up
 */
static void uninit(void);

static int init(void)
{
    error e;
    if (e = ::audio_init())
        goto ret;

    if (e = ::tts_init())
        goto ret;

ret:
    std::cerr << "init : " << ::error_what(e) << std::endl;
    return e;
}

static int setupaudio(audio_engine*& ptra)
{
    error e;

    audio_engine* o;

    audio_drivermeta const* metas;
    size_t noMeta;

    size_t iSelected;

    auto seDm = ScopeExit(
        [&metas](void) -> void
            { ::audio_freedrivermeta(metas); }
    );

    if (e = ::audio_putdrivermeta(&metas, &noMeta))
        goto ret;

    std::cout << "pick one from" << std::endl;
    for (size_t i = 0; i < noMeta; i++) {
        audio_drivermeta const& dm = metas[i];
        std::cout << "[" << i << "] - " << dm.name << std::endl;
    }

    iSelected = noMeta;
    while (iSelected >= noMeta)
        iSelected = (std::getchar() - '0');

    if (e = ::audio_createenginewith(metas[iSelected], &o))
        goto ret;

ret:
    std::cerr << "setup : " << ::error_what(e) << std::endl;
    return e;
}

static int startkernel(void)
    { return 0; }

static void uninit(void)
    { ; }

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
    if (o = ::setupaudio(a))
        return o;

    if (o = ::startkernel())
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
