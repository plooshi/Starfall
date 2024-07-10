#pragma once
#include "pch.h"
#include "ue.h"
#include "url.h"

#define CallVirt(T, vt, offset, ...) ((T) vt[offset])(__VA_ARGS__)

namespace Unreal {
    class FHttpRequestWinInet
    {
    public:
        void** VTable;
        static inline FString& (*GetURLFunc)(FHttpRequestWinInet*, FString) = nullptr;
        static inline void (*SetURLFunc)(FHttpRequestWinInet*, FString) = nullptr;
        static inline void** ProcessRequestVT = nullptr;

        FString GetURL();
        void SetURL(URL& URL);
    };
}

namespace Starfall {
    extern FString backend; // for phoenix/paradise launcher
    void SetupRequest(FHttpRequestWinInet* Request);
     
    namespace Hooks {
        bool ProcessRequestHook(FHttpRequestWinInet* Request);
    }
    using namespace Hooks;

    namespace Callbacks {
        bool PtrCallback(struct pf_patch_t* patch, void* stream);

        bool StringCallback(struct pf_patch_t* patch, void* stream);
    }
    using namespace Callbacks;

    namespace Finders {
        void FindProcessRequest();
    }
    using namespace Finders;
}