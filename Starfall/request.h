#pragma once
#include "pch.h"
#include "ue.h"
#include "url.h"

#define CallVirt(T, vt, offset, ...) ((T) vt[offset])(__VA_ARGS__)

namespace Unreal {
    class FCurlHttpRequest
    {
    public:
        void** VTable;
        static inline int64_t SetURLIdx;
        static inline void** ProcessRequestVT = nullptr;

        FString GetURL();
    };
}

namespace Starfall {
    extern FString backend; // for phoenix/paradise launcher
    void SetupRequest(FCurlHttpRequest* Request);
     
    namespace Hooks {
        bool ProcessRequestHook(FCurlHttpRequest* Request);
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