/* { audio_wasapi.cpp } : audio wsasapi backend impl
 */

extern "C" {

    #include "audio.h"

    #include "annotations.h"

extern "C++" {

    #pragma comment(lib, "Avrt.lib")

    #include <windows.h>
    #include <mmdeviceapi.h>
    #include <audioclient.h>
    #include <avrt.h>
    #include <Functiondiscoverykeys_devpkey.h>

    #include "expected.hpp"
    #include "scopeexit.hpp"

namespace {

namespace wasapi {

    using MM_DEVICE_ENUMERATOR    = IMMDeviceEnumerator;
    using MM_DEVICE               = IMMDevice;
    using WAS_AUDIO_CLIENT        = IAudioClient;
    using WAS_AUDIO_RENDER_CLIENT = IAudioRenderClient;

    struct ENGINE final {
        audio_engine engine;
    };

    FN_NOTIMPLEMENTED_PRIORITYMAX
    Expected<MM_DEVICE_ENUMERATOR*, error> createdeviceenumerator(void);

    FN_NOTIMPLEMENTED_PRIORITYMAX
    Expected<audio_drivermeta*, error> enumerateendpoints(
        MM_DEVICE_ENUMERATOR*
    );

    FN_NOTIMPLEMENTED_PRIORITYMAX
    Expected<audio_drivermeta, error> dmfromdevice(MM_DEVICE*);

} // wasapi

    /*
     * still qualify namespace for function symbols because. Its buggin out
     */
    using namespace wasapi;

    Expected<MM_DEVICE_ENUMERATOR*, error> wasapi::createdeviceenumerator(void)
    {
        HRESULT hr;
        MM_DEVICE_ENUMERATOR* o;

        auto constexpr rclsidEnumerator = __uuidof(MMDeviceEnumerator);
        hr = ::CoCreateInstance(
            rclsidEnumerator, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&o)
        );
        if FAILED(hr)
            return ::error_errorfromhr(hr);

        return o;
    }

    Expected<audio_drivermeta*, error> wasapi::enumerateendpoints(
        MM_DEVICE_ENUMERATOR* e
    )
    {
        HRESULT hr;
        audio_drivermeta* o;

        IMMDeviceCollection* coll;

        hr = e->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &coll);
        if FAILED(hr)
            return ::error_errorfromhr(hr);

        auto seColl = ScopeExit(
            [&coll](void) -> void
                { coll->Release(); }
        );

        UINT no;
        coll->GetCount(&no);

        /*
         * + 1 for the sentinel
         */
        o = new audio_drivermeta[no + 1];
        ::memset(o, 0x00, ((no + 1) * (sizeof * o)));

        auto seDm = ScopeExit(
            [&o, no](void) -> void
            {
                u32_t i = 0;
                for (;;) {
                    if (i > no)
                        break;

                    if (o[i].driver == nullptr)
                        break;

                    static_cast<MM_DEVICE*>(o[i].driver)->Release();
                    i++;
                }

                delete[] o;
            }
        );

        for (UINT i = 0; i < no; i++) {
            MM_DEVICE* device;
            hr = coll->Item(i, &device);
            if FAILED(hr)
                return ::error_errorfromhr(hr);

            auto dm = ::dmfromdevice(device);
            if (!dm)
                return dm.Error();
            o[i] = *dm;
        }

        seDm.Cancel();
        return o;
    }

    Expected<audio_drivermeta, error> wasapi::dmfromdevice(MM_DEVICE* device)
    {
        HRESULT hr;
        audio_drivermeta o = { };

        /*
         * https://learn.microsoft.com/en-us/windows/win32/api/mmdeviceapi/nf-mmdeviceapi-immdeviceenumerator-getdevice
         * so individual enum|device refs can be released For Free
         */
        hr = device->GetId(reinterpret_cast<LPWSTR*>(&o.driver));
        if FAILED(hr)
            return ::error_errorfromhr(hr);

        IPropertyStore* props;
        hr = device->OpenPropertyStore(STGM_READ, &props);
        if FAILED(hr)
            return ::error_errorfromhr(hr);

        PROPVARIANT name;
        ::PropVariantInit(&name);
        hr = props->GetValue(PKEY_Device_FriendlyName, &name);
        if (!SUCCEEDED(hr))
            return ::error_errorfromhr(hr);

        ::WideCharToMultiByte(
            CP_UTF8, 0, name.pwszVal, -1,
            o.name, sizeof o.name,
            nullptr, nullptr
        );

        hr = ::PropVariantClear(&name);
        if FAILED(hr)
            return ::error_errorfromhr(hr);

        return o;
    }

} // {unnamed}

} // extern "C++"

    error audio_init(void)
    {
        HRESULT hr;

        hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
        if FAILED(hr)
            return ::error_errorfromhr(hr);

        return error_ok;
    }

    void audio_uninit(void)
        { ::CoUninitialize(); }

    error audio_putdrivermeta(audio_drivermeta const** ptrdms, size_t* no)
    {
        auto enumerator = wasapi::createdeviceenumerator();
        if (!enumerator)
            return enumerator.Error();

        auto seEnumerator = ScopeExit(
            [&enumerator](void) -> void
                { (*enumerator)->Release(); }
        );

        auto metas = wasapi::enumerateendpoints(*enumerator);
        if (!metas)
            return metas.Error();

        *ptrdms = *metas;
        return error_ok;
    }

    void audio_freedrivermeta(struct audio_drivermeta const* dms)
    {
        (void)dms;
        ;
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
        ;
    }

} // extern "C"
