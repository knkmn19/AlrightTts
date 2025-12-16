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
static int setupaudio(void);

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
    { return 0; }

static int setupaudio(void)
    { return 0; }

static int startkernel(void)
    { return 0; }

static void uninit(void)
    { ; }

int main(int, char** vector)
{
    int o;

    (void)vector;

    o = ::init();
    if (o = ::init())
        return o;

    auto se = ScopeExit(
        [](void) -> void
            { ::uninit(); }
    );

    if (o = ::setupaudio())
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
