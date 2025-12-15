/* { audio_wasapi.cpp } : audio wsasapi backend impl
 */

extern "C" {

    #include "audio.h"

extern "C++" {

    #pragma comment(lib, "Avrt.lib")

    #include <windows.h>
    #include <mmdeviceapi.h>
    #include <audioclient.h>
    #include <avrt.h>
    #include <Functiondiscoverykeys_devpkey.h>

namespace {

    using MM_DEVICE_ENUMERATOR    = IMMDeviceEnumerator;
    using MM_DEVICE               = IMMDevice;
    using WAS_AUDIO_CLIENT        = IAudioClient;
    using WAS_AUDIO_RENDER_CLIENT = IAudioRenderClient;

    struct ENGINE final {
        audio_engine engine;
    };

} // {unnamed}

} // extern "C++"

    error audio_putdrivermeta(audio_drivermeta const** ptrdms)
    {
        HRESULT hr;

        /*
         * setup com here for now
         */
        hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
        if FAILED(hr)
            return ::error_errorfromhr(hr);



        *ptrdms = NULL;
        return error_ok;
    }

    void audio_freedrivermeta(audio_drivermeta const** ptrdms)
    {
        audio_drivermeta const* dm = *ptrdms;
        for (;;) {
            if (dm == NULL)
                break;
            (void)dm;
            dm += 1;
        }
    }

    error audio_createenginewith(
        audio_drivermeta const dm, audio_engine** ptra
    )
    {
        (void)dm, (void)ptra;
        return error_fail;
    }

    void audio_destroyengine(audio_engine* a)
    {
        (void)a;
        /*
         * and uninit com here
         */
        ::CoUninitialize();
    }

} // extern "C"
