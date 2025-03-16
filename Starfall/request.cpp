#include "pch.h"
#include "base.h"
#include "hooking.h"
#include "url.h"
#include "request.h"
#include "redirection.h"
#include <array>
#include "opts.h"
#include <time.h>
#include <sys/timeb.h>

#define CallVirt(T, vt, offset, ...) ((T) vt[offset])(__VA_ARGS__)

namespace Unreal {
    FString FCurlHttpRequest::GetURL()
    {
        return ((FString&(*)(FCurlHttpRequest*, FString)) VTable[0])(this, FString());
    }
}

namespace Starfall {
    bool setupMemLeak = false;
    void SetupRequest(FCurlHttpRequest* Request) {
        if (FCurlHttpRequest::SetURLIdx == 0) {
            void* GetFunc = *Request->VTable;
            uint32_t URLOffset = 0;
            for (int i = 0; i < 100; i++) {
                // this is only needed for latest
                if (CheckBytes<0x48, 0x8D, 0x91>(GetFunc, i)) {
                    URLOffset = *(uint32_t*)(__int64(GetFunc) + i + 3);
                    break;
                }
            }

            if (URLOffset == 0) goto def;
            for (int64_t i = 0; i < ((__int64(FCurlHttpRequest::ProcessRequestVT) - __int64(Request->VTable)) / 8) /* search radius */; i++) {
                auto func = Request->VTable[i];
                for (int j = 0; j < 100; j++) {
                    if (CheckBytes<0x48, 0x81, 0xC1>(func, j)) {
                        if (*(uint32_t*)(__int64(func) + j + 3) == URLOffset) {
                            FCurlHttpRequest::SetURLIdx = i;
                            return;
                        }
                    }
                }
            }
def:
            FCurlHttpRequest::SetURLIdx = 10;
        }
        // this works bc the first request is a datarouter request, and the second request should be after engine init
        else if (!setupMemLeak && FixMemLeak) {
            constexpr static struct pf_patch_t ml_patch = pf_construct_patch_sig("48 8B 01 4C 8D 41 08 48 FF 60 20", Ret0Callback);
            constexpr static struct pf_patch_t ml_patch2 = pf_construct_patch_sig("48 89 5C 24 ?? 57 48 83 EC ?? 48 8B 01 4C 8B C2 48 8D 54 24", Ret0Callback);

            constexpr static struct pf_patch_t patches[] = {
                ml_patch,
                ml_patch2
            };

            constexpr static struct pf_patchset_t patchset = pf_construct_patchset(patches, sizeof(patches) / sizeof(struct pf_patch_t));

            pf_patchset_emit(tbuf, tsize, patchset);
            setupMemLeak = true;
        }
    }

    FString backend;

    namespace Hooks {
        bool (*ProcessRequestOG)(FCurlHttpRequest* Request);
        bool (*EOSProcessRequestOG)(FCurlHttpRequest* Request);
        bool InternalProcessRequest(FCurlHttpRequest* Request, decltype(ProcessRequestOG) OG) {
            SetupRequest(Request);
            auto urlS = Request->GetURL();
            auto url = (URL*)_malloca(sizeof(URL));
            if (!url) return false;
            url->Construct(urlS);

            Log(Display, "URL: %ls\n", static_cast<wchar_t*>(urlS));
            if (shouldRedirect(url)) {
                if (UseBackendParam)
                    url->SetHost(backend);
                else {
                    __URL_SetHost(url, Backend);
                }

                FString str = *url;
                ((void (*)(FCurlHttpRequest*, FString)) Request->VTable[OG == EOSProcessRequestOG ? 10 : FCurlHttpRequest::SetURLIdx])(Request, str);
                free(str.String);

                UseBackendParam ? url->Dealloc() : url->DeallocPathQuery();
            }
            else {
                url->Dealloc();
            }
              
            return OG(Request);
        }
        bool ProcessRequestHook(FCurlHttpRequest* Request) {
            return InternalProcessRequest(Request, ProcessRequestOG);
        }

        bool EOSProcessRequestHook(FCurlHttpRequest* Request) {
            return InternalProcessRequest(Request, EOSProcessRequestOG);
        }
    }

    namespace Callbacks {
        bool PtrCallback(struct pf_patch_t* patch, void* stream) {
            FCurlHttpRequest::ProcessRequestVT = (void**)stream;

            VTHook((void**)stream, ProcessRequestHook, (void **) &ProcessRequestOG);
            return true;
        }

        bool EOSPtrCallback(struct pf_patch_t* patch, void* stream) {
            VTHook((void**)stream, EOSProcessRequestHook, (void**)&EOSProcessRequestOG);
            return true;
        }


        constexpr static char ptrMasks[] = {
            (char)0xff,
            (char)0xff,
            (char)0xff,
            (char)0xff,
            (char)0xff,
            (char)0xff,
            (char)0xff,
            (char)0xff
        };


        __forceinline void* checkStream(void *stream, bool bEOS) {
            for (int i = 0; i < 2048; i++) {
                if (bEOS)
                {
                    if (CheckBytes<0x48, 0x89, 0x5C>(stream, i, true)) {
                        Log(Display, "Found with 49 89 5C, offset: %llx\n", __int64(stream) - i - __int64(buf));
                        goto setStream;
                    }
                }
                else
                {
                    if (CheckBytes<0x4C, 0x8B, 0xDC>(stream, i, true)) {
                        Log(Display, "Found with 4C 8B DC, offset: %llx\n", __int64(stream) - i - __int64(buf));
                        goto setStream;
                    }
                    else if (CheckBytes<0x48, 0x8B, 0xC4>(stream, i, true)) {
                        Log(Display, "Found with 48 8B C4, offset: %llx\n", __int64(stream) - i - __int64(buf));
                    setStream:
                        return (uint8_t*)stream - i;
                    }
                    else if (CheckBytes<0x48, 0x81, 0xEC>(stream, i, true) || CheckBytes<0x48, 0x83, 0xEC>(stream, i, true)) {
                        for (int x = 0; x < 50; x++) {
                            if (CheckBytes<0x40>(stream, i + x, true)) {
                                Log(Display, "Found with 40, offset: %llx\n", __int64(stream) - i - x - __int64(buf));
                                return (uint8_t*)stream - i - x;
                            }
                            else if (CheckBytes<0x4C, 0x8B, 0xDC>(stream, i + x, true) || CheckBytes<0x48, 0x8B, 0xC4>(stream, i + x, true) || CheckBytes<0x48, 0x89, 0x5C>(stream, i + x, true))
                                break;
                        }
                    }
                }
            }
            return nullptr;
        }

        bool InternalCallback(void* stream, void* rbuf, size_t rsize, bool (*callback)(pf_patch_t*, void*), bool bEOS) {
            void* saddr = (void*)((__int64(stream) + 7) + *(int32_t*)(__int64(stream) + 3));
            void* newStream = nullptr;
            if (__int64(saddr) >= __int64(rbuf) && __int64(saddr) < (__int64(rbuf) + (int64_t)rsize))
                if (wcscmp((wchar_t*)saddr, L"STAT_FCurlHttpRequest_ProcessRequest") == 0 || wcscmp((wchar_t*)saddr, L"%p: request (easy handle:%p) has been added to threaded queue for processing") == 0)
                    if (newStream = checkStream(stream, bEOS)) goto Out;
            return false;
        Out:
            char* ptrMatches = (char*)&newStream;

            auto patch2 = pf_construct_patch(ptrMatches, (void*)ptrMasks, 8, callback);

            struct pf_patch_t patches2[] = {
                patch2
            };

            struct pf_patchset_t patchset2 = pf_construct_patchset(patches2, sizeof(patches2) / sizeof(struct pf_patch_t));
            while (!pf_patchset_emit(rbuf, rsize, patchset2));
            return true;
        }
        bool StringCallback(struct pf_patch_t* patch, void* stream) {
            return InternalCallback(stream, rbuf, rsize, PtrCallback, false);
        }

        bool EOSStringCallback(struct pf_patch_t* patch, void* stream) {
            return InternalCallback(stream, EOSRDataBuf, EOSRDataSize, EOSPtrCallback, true);
        }
    }

    namespace Finders {
        void FindProcessRequest() {
            constexpr static std::array<uint8_t, 2> matches = {
                0x48,
                0x8d
            };
            constexpr static std::array<uint8_t, 2> masks = {
                0xfb,
                0xff
            };
            {
                constexpr static auto patch = pf_construct_patch((void*)matches.data(), (void*)masks.data(), 2, StringCallback);

                constexpr static pf_patch_t patches[] = {
                    patch
                };

                constexpr static struct pf_patchset_t patchset = pf_construct_patchset(patches, sizeof(patches) / sizeof(struct pf_patch_t));

                while (!pf_patchset_emit(tbuf, tsize, patchset));
            }

            if (EOSBuf) {

                constexpr static auto patch = pf_construct_patch((void*)matches.data(), (void*)masks.data(), 2, EOSStringCallback);

                constexpr static pf_patch_t patches[] = {
                    patch
                };

                constexpr static struct pf_patchset_t patchset = pf_construct_patchset(patches, sizeof(patches) / sizeof(struct pf_patch_t));

                while (!pf_patchset_emit(EOSTextBuf, EOSTextSize, patchset));
            }
        }
    }
}