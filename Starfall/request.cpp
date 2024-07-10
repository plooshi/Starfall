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
    FString FHttpRequestWinInet::GetURL()
    {
        return GetURLFunc(this, FString());
    }

    void FHttpRequestWinInet::SetURL(URL& URL)
    {
        FString str = URL;
        SetURLFunc(this, str);
        free(str.String);
    }
}

namespace Starfall {
    bool setupMemLeak = false;
    void SetupRequest(FHttpRequestWinInet* Request) {
        if (FHttpRequestWinInet::SetURLFunc == nullptr) {
            void* GetFunc = *Request->VTable;
            FHttpRequestWinInet::GetURLFunc = (decltype(FHttpRequestWinInet::GetURLFunc)) *Request->VTable;
            uint32_t URLOffset = 0;
            for (int i = 0; i < 100; i++) {
                // this is only needed for latest
                if (CheckBytes<0x48, 0x8D, 0x91>(GetFunc, i)) {
                    URLOffset = *(uint32_t*)(__int64(GetFunc) + i + 3);
                    break;
                }
            }

            if (URLOffset == 0) goto def;
            for (int64_t i = 0; i < ((__int64(FHttpRequestWinInet::ProcessRequestVT) - __int64(Request->VTable)) / 8) /* search radius */; i++) {
                auto func = Request->VTable[i];
                for (int j = 0; j < 100; j++) {
                    if (CheckBytes<0x48, 0x81, 0xC1>(func, j)) {
                        if (*(uint32_t*)(__int64(func) + j + 3) == URLOffset) {
                            FHttpRequestWinInet::SetURLFunc = (void (*)(FHttpRequestWinInet*, FString)) Request->VTable[i];
                            return;
                        }
                    }
                }
            }
def:
            FHttpRequestWinInet::SetURLFunc = (void (*)(FHttpRequestWinInet*, FString)) Request->VTable[10];
        }
        // this works bc the first request is a datarouter request, and the second request should be after engine init
        else if (!setupMemLeak && Game == Fortnite) {
            constexpr static struct pf_patch_t ml_patch = pf_construct_patch_sig("4C 8B DC 55 57 41 56 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 01 41 B6", Ret0Callback);

            constexpr static struct pf_patch_t patches[] = {
                ml_patch
            };

            constexpr static struct pf_patchset_t patchset = pf_construct_patchset(patches, sizeof(patches) / sizeof(struct pf_patch_t), (bool (*)(void*, size_t, pf_patchset_t))pf_find_maskmatch);

            pf_patchset_emit(tbuf, tsize, patchset);
            setupMemLeak = true;
        }
    }

    FString backend;

    namespace Hooks {
        bool (*ProcessRequestOG)(FHttpRequestWinInet* Request);
        bool ProcessRequestHook(FHttpRequestWinInet* Request) {
            SetupRequest(Request);
            auto urlS = Request->GetURL();
            auto url = (URL *) malloc(sizeof(URL));
            if (!url) return false;
            __stosb((uint8_t*)url, 0, sizeof(URL));
            url->Construct(urlS);

            Log(Display, "URL: %ls\n", static_cast<wchar_t *>(urlS));
            if (shouldRedirect(url)) {
                Request->SetURL(UseBackendParam ? url->SetHost(backend) : url->SetHost<Backend>());

                UseBackendParam ? url->Dealloc() : url->DeallocPathQuery();
            }
            else {
                url->Dealloc();
            }

            free(url);
            return ProcessRequestOG(Request);
        }
    }

    namespace Callbacks {
        bool PtrCallback(struct pf_patch_t* patch, void* stream) {
            FHttpRequestWinInet::ProcessRequestVT = (void**)stream;

            Log(Display, "ProcessRequestVT: 0x%llx\n", __int64(stream) - __int64(buf));
            VTHook((void**)stream, ProcessRequestHook, (void **) &ProcessRequestOG);
            return true;
        }

        bool StringCallback(struct pf_patch_t* patch, void* stream) {
            void* saddr = (void*)((__int64(stream) + 7) + *(int32_t*)(__int64(stream) + 3));
            if (__int64(saddr) >= __int64(rbuf) && __int64(saddr) < (__int64(rbuf) + (int64_t)rsize)) {
                if (wcscmp((wchar_t*)saddr, L"ProcessRequest failed. Could not initialize Internet connection.") == 0) {
                    for (int i = 0; i < 2048; i++) {
                        if (CheckBytes<0x48, 0x81, 0xEC>(stream, i, true)) {
                            for (int x = 0; x < 50; x++) {
                                if (CheckBytes<0x40>(stream, i + x, true)) {
                                    Log(Display, "Found ProcessRequest!\n");
                                    stream = (uint8_t*)stream - i - x;
                                    goto HookVT;
                                }
                            }
                        }
                    }
                }
            }
            return false;
        HookVT:
            Log(Display, "ProcessRequest: 0x%llx\n", __int64(stream) - __int64(buf));
            char* ptrMatches = (char*)&stream;

#ifndef _DEBUG
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

            constexpr static auto patch2 = pf_construct_patch_dynmatch((void*)ptrMasks, 8, PtrCallback);

            constexpr static struct pf_patch_t patches2[] = {
                patch2
            };

            constexpr static struct pf_patchset_t patchset2 = pf_construct_patchset(patches2, sizeof(patches2) / sizeof(struct pf_patch_t), (bool (*)(void*, size_t, pf_patchset_t))pf_find_maskmatch);

            auto& patchToMod = (pf_patch_t &) patch2;
            DWORD og;
            VirtualProtect((void*)&(patchToMod.matches), sizeof(void*), PAGE_READWRITE, &og);
            patchToMod.matches = ptrMatches;
            VirtualProtect((void*)&(patchToMod.matches), sizeof(void*), og, &og);
#else
            char ptrMasks[] = {
                (char)0xff,
                (char)0xff,
                (char)0xff,
                (char)0xff,
                (char)0xff,
                (char)0xff,
                (char)0xff,
                (char)0xff
            };

            auto patch2 = pf_construct_patch(ptrMatches, (void*)ptrMasks, 8, PtrCallback);

            struct pf_patch_t patches2[] = {
                patch2
            };

            struct pf_patchset_t patchset2 = pf_construct_patchset(patches2, sizeof(patches2) / sizeof(struct pf_patch_t), (bool (*)(void*, size_t, pf_patchset_t))pf_find_maskmatch);
#endif
            while (!pf_patchset_emit(rbuf, rsize, patchset2));
            return true;
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

            constexpr static auto patch = pf_construct_patch((void*)matches.data(), (void*)masks.data(), 2, StringCallback);

            constexpr static pf_patch_t patches[] = {
                patch
            };

            constexpr static struct pf_patchset_t patchset = pf_construct_patchset(patches, sizeof(patches) / sizeof(struct pf_patch_t), pf_find_maskmatch);

            while (!pf_patchset_emit(tbuf, tsize, patchset));
        }
    }
}