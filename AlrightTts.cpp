// AlrightTts.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

extern "C" {
    #include "audio.h"
    #include "tts.h"
} // extern "C"

#include <iostream>
#include "scopeexit.hpp"

int main(int, char** vector)
{
    (void)vector;

    if (auto e = ::tts_init())
        return (std::cout << ::error_what(e) << std::endl, e);

    auto seTts = ScopeExit(
        [](void) -> void
            { ::tts_destroy(); }
    );

    audio_drivermeta const* metas;
    if (auto e = ::audio_putdrivermeta(&metas))
        return (std::cout << ::error_what(e) << std::endl, e);

    auto seDm = ScopeExit(
        [&metas](void) -> void
            { ::audio_freedrivermeta(&metas); }
    );

    for (size_t i = 0; (metas + i)->driver != nullptr; i++) {
        auto const* dm = (metas + i);
        std::cout << dm->name << std::endl;
    }

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
