/* { audio_wasapi.cpp } : audio wsasapi backend impl
 */

extern "C" {

    #include "audio.h"

    #include "annotations.h"

extern "C++" {

    #pragma comment(lib, "Avrt.lib")
    #pragma comment(lib, "Synchronization.lib")

    #include <windows.h>
    #include <process.h>
    #include <mmdeviceapi.h>
    #include <audioclient.h>
    #include <avrt.h>
    #include <Functiondiscoverykeys_devpkey.h>

    #include <new>
    #include <cassert>
    #include "expected.hpp"
    #include "scopeexit.hpp"

namespace {

    template<typename T>
    void writeandwake(T& where, T const);

namespace wasapi {

    using MM_DEVICE_ENUMERATOR    = IMMDeviceEnumerator;
    using MM_DEVICE               = IMMDevice;
    using WAS_AUDIO_CLIENT        = IAudioClient;
    using WAS_AUDIO_RENDER_CLIENT = IAudioRenderClient;

    struct ENGINE final {
        audio_engine mutable engine;

        error errorThrd;
        uintptr_t thrdAudio;
        /*
         * prefer bool_t cause 4 bytes is easier to align than 1
         */
        bool_t mutable bShouldExit;

        WAS_AUDIO_CLIENT* client;
        HANDLE eventCallback;
        WAS_AUDIO_RENDER_CLIENT* renderclient;

        FN_NOTIMPLEMENTED_PRIORITYMAX
        void static main(void* engine);
        error SetupClient(void);
        FN_NOTIMPLEMENTED_PRIORITYMAX
        error Mix(void) const;

        error Initialize(audio_drivermeta const&);
    };

    struct ENDPOINTS_METADATA final {
        audio_drivermeta* metas;
        size_t noMetas;
    };

    Expected<MM_DEVICE_ENUMERATOR*, error> createdeviceenumerator(void);

    Expected<ENDPOINTS_METADATA, error> enumerateendpoints(
        MM_DEVICE_ENUMERATOR*
    );

    Expected<audio_drivermeta, error> dmfromdevice(MM_DEVICE*);

    Expected<MM_DEVICE*, error> createdevice(LPCWSTR guid);

    Expected<WAS_AUDIO_CLIENT*, error> createclient(MM_DEVICE*, HANDLE event);

    WAVEFORMATEXTENSIBLE preferredmixformatof(WAS_AUDIO_CLIENT*);

    Expected<WAS_AUDIO_RENDER_CLIENT*, error> queryrenderclientfrom(
        WAS_AUDIO_CLIENT*
    );

} // wasapi

    /*
     * still qualify namespace for function symbols because. Its buggin out
     */
    using namespace wasapi;

    template<typename T>
    void writeandwake(T& where, T const v)
    {
        where = v;
        ::WakeByAddressSingle(&where);
    }

    void wasapi::ENGINE::main(void* p)
    {
        error e;
        auto& engine = *static_cast<wasapi::ENGINE*>(p);

        auto seWriteE = ScopeExit(
            [&engine, &e](void) -> void
                { ::writeandwake(engine.errorThrd, e); }
        );

        if (e = engine.SetupClient())
            return;

        auto seStop = ScopeExit(
            [&engine](void) -> void
                { (void)engine.client->Stop(); }
        );

        if (e = engine.Mix())
            return;
    }

    error wasapi::ENGINE::SetupClient(void)
    {
        auto guidDevice = static_cast<LPCWSTR>(this->engine.drivermeta.driver);
        auto device = wasapi::createdevice(guidDevice);
        if (!device)
            return device.Error();

        auto seDevice = ScopeExit(
            [&device](void) -> void
                { (*device)->Release(); }
        );

        HANDLE event = ::CreateEvent(nullptr, false, false, nullptr);
        if (event == NULL)
            return ::error_errorfromhr(::HRESULT_FROM_WIN32(::GetLastError()));
        this->eventCallback = event;

        auto seEvent = ScopeExit(
            [&event](void) -> void
                { ::CloseHandle(event); }
        );

        auto client = wasapi::createclient(*device, event);
        if (!client)
            return client.Error();
        this->client = *client;

        auto renderclient = wasapi::queryrenderclientfrom(*client);
        if (!renderclient)
            return renderclient.Error();
        this->renderclient = *renderclient;

        seEvent.Cancel();
        return error_ok;
    }

    error wasapi::ENGINE::Mix(void) const
    {
        HRESULT hr;

        UINT32 register szBuf;
        hr = this->client->GetBufferSize(&szBuf);
        if (FAILED(hr))
            return ::error_errorfromhr(hr);

        for (;;) {
            bool const bExit = ::InterlockedCompareExchange(
                reinterpret_cast<dword_t*>(&this->bShouldExit), 0u, 0u
            );
            if CMP_UNLIKELY (bExit)
                break;

            ::WaitForSingleObject(this->eventCallback, INFINITE);
            bool const bPlaying = ::InterlockedCompareExchange(
                reinterpret_cast<dword_t*>(&this->engine.bplaying), 0u, 0u
            );

            UINT32 pad;
            hr = this->client->GetCurrentPadding(&pad);
            if CMP_UNLIKELY (FAILED(hr))
                return ::error_errorfromhr(hr);

            size_t const szPacket = (szBuf - pad);
            if CMP_UNLIKELY (szPacket == 0)
                continue;

            byte_t* packet;
            hr = this->renderclient->GetBuffer(szPacket, &packet);
            if CMP_UNLIKELY (FAILED(hr))
                return ::error_errorfromhr(hr);
            {
                ::memset(packet, 0x00, szPacket);
                if (bPlaying) {
                    ;
                    ;
                }
            }
            hr = this->renderclient->ReleaseBuffer(szPacket, 0);
            if CMP_UNLIKELY (FAILED(hr))
                return ::error_errorfromhr(hr);
        }

        return error_ok;
    }

    error wasapi::ENGINE::Initialize(audio_drivermeta const& dm)
    {
        this->errorThrd = error_unset;
        this->engine.drivermeta = dm;

        if (::_beginthread(ENGINE::main, 0, this) == -1L)
            return error_badalloc;

        error errorStale = error_unset;
        error errorReal;
        for (;;) {
            BOOL b = ::WaitOnAddress(
                &this->errorThrd, &errorStale, sizeof errorStale, INFINITE
            );

            errorReal = static_cast<error>(
                ::InterlockedCompareExchange(
                    reinterpret_cast<LONG*>(&this->errorThrd), 0u, 0u
                )
            );
            if (errorReal != errorStale)
                /*
                 * safe to clean up stack and leave
                 */
                break;
        }

        return errorReal;
    }

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

    Expected<ENDPOINTS_METADATA, error> wasapi::enumerateendpoints(
        MM_DEVICE_ENUMERATOR* e
    )
    {
        HRESULT hr;
        ENDPOINTS_METADATA o;

        IMMDeviceCollection* coll;
        hr = e->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &coll);
        if FAILED(hr)
            return ::error_errorfromhr(hr);

        UINT no;
        (void)coll->GetCount(&no);

        auto seColl = ScopeExit(
            [&coll, no](void) -> void
            {
                for (UINT i = 0; i < no; i++) {
                    MM_DEVICE* device;
                    (void)coll->Item(i, &device);

                    if (auto* p = device)
                        p->Release();
                }

                coll->Release();
            }
        );

        auto* metas = new (std::nothrow) audio_drivermeta[no];
        if (metas == nullptr)
            return error_badalloc;
        ::memset(metas, 0x00, (no * (sizeof * metas)));

        auto seDm = ScopeExit(
            [&metas](void) -> void
                { delete[] metas; }
        );

        for (UINT i = 0; i < no; i++) {
            MM_DEVICE* device;
            hr = coll->Item(i, &device);
            if FAILED(hr)
                return ::error_errorfromhr(hr);

            auto dm = ::dmfromdevice(device);
            if (!dm)
                return dm.Error();
            metas[i] = *dm;
        }

        o.metas = metas;
        o.noMetas = no;

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

    Expected<MM_DEVICE*, error> wasapi::createdevice(LPCWSTR guid)
    {
        HRESULT hr;
        MM_DEVICE* o;

        auto enumerator = wasapi::createdeviceenumerator();
        if (!enumerator)
            return enumerator.Error();

        auto seEnumerator = ScopeExit(
            [&enumerator](void) -> void
                { (*enumerator)->Release(); }
        );

        hr = (*enumerator)->GetDevice(guid, &o);
        if FAILED(hr)
            return ::error_errorfromhr(hr);

        return o;
    }

    Expected<WAS_AUDIO_CLIENT*, error> wasapi::createclient(
        MM_DEVICE* device, HANDLE event
    )
    {
        HRESULT hr;
        WAS_AUDIO_CLIENT* o;

        hr = device->Activate(
            __uuidof(*o), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&o)
        );
        if FAILED(hr)
            return ::error_errorfromhr(hr);

        WAVEFORMATEXTENSIBLE wfe = wasapi::preferredmixformatof(o);
        hr = o->Initialize(
            AUDCLNT_SHAREMODE_SHARED,
            AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 0, 0, &wfe.Format,
            NULL
        );
        if FAILED(hr)
            return ::error_errorfromhr(hr);

        hr = o->SetEventHandle(event);
        if FAILED(hr)
            return ::error_errorfromhr(hr);

        hr = o->Start();
        if FAILED(hr)
            return ::error_errorfromhr(hr);

        return o;
    }

    WAVEFORMATEXTENSIBLE wasapi::preferredmixformatof(WAS_AUDIO_CLIENT* client)
    {
        HRESULT hr;
        WAVEFORMATEXTENSIBLE o = { };

        WAVEFORMATEX* wf;
        hr = client->GetMixFormat(&wf);
        if FAILED(hr)
            /*
             * we arent really gonna handle an error (its really unlikely
             */
            return (assert(false), o);

        if (wf->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
            o = *reinterpret_cast<WAVEFORMATEXTENSIBLE*>(wf);
        else
            o.Format = *wf;

        ::CoTaskMemFree(wf);

        return o;
    }

    Expected<WAS_AUDIO_RENDER_CLIENT*, error> wasapi::queryrenderclientfrom(
        WAS_AUDIO_CLIENT* client
    )
    {
        HRESULT hr;
        WAS_AUDIO_RENDER_CLIENT* o;

        hr = client->GetService(IID_PPV_ARGS(&o));
        if FAILED(hr)
            return ::error_errorfromhr(hr);

        /*
         * make the beginning silent just for safety
         */
        UINT32 szBuffer;
        hr = client->GetBufferSize(&szBuffer);
        if FAILED(hr)
            return ::error_errorfromhr(hr);

        byte_t* _;
        hr = o->GetBuffer(szBuffer, &_);
        if FAILED(hr)
            return ::error_errorfromhr(hr);
        hr = o->ReleaseBuffer(szBuffer, AUDCLNT_BUFFERFLAGS_SILENT);
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

    error audio_putdrivermeta(audio_drivermeta const** ptrdms, size_t* ptrno)
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

        *ptrdms = (*metas).metas;
        *ptrno = (*metas).noMetas;
        return error_ok;
    }

    void audio_freedrivermeta(struct audio_drivermeta const* dms)
    {
        if (auto* p = dms)
            delete[] p;
    }

    error audio_createenginewith(
        audio_drivermeta const dm, audio_engine** ptra
    )
    {
        error e;
        (void)dm;

        auto* o = new (std::nothrow) wasapi::ENGINE;
        if (o == nullptr)
            return error_badalloc;
        ::memset(o, 0x00, sizeof *o);

        auto seDestroy = ScopeExit(
            [&o](void) -> void
                { delete o; }
        );

        if (e = o->Initialize(dm))
            return e;

        seDestroy.Cancel();
        *ptra = reinterpret_cast<audio_engine*>(o);
        return error_ok;
    }

    void audio_destroyengine(audio_engine* a)
    {
        if (auto* p = reinterpret_cast<wasapi::ENGINE*>(a))
            delete a;
    }

} // extern "C"
