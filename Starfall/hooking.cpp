#pragma once
#include "pch.h"
#include "hooking.h"

namespace Starfall {
    namespace Hooking {
        __forceinline void AsmHook(uint64_t ptr, void* detour) {
            AsmHook((void*)ptr, detour);
        }

        bool Ret0Callback(struct pf_patch_t* patch, void* stream) {
            DWORD og;
            VirtualProtect(stream, 1, PAGE_EXECUTE_READWRITE, &og);
            *(uint8_t*)stream = (uint8_t)0xC3;
            VirtualProtect(stream, 1, og, &og);
            return true;
        }

        void VTHook(void** addr, void* detour, void** orig) {
            DWORD oldProt;

            VirtualProtect(addr, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProt);

            if (orig) *orig = *addr;

            *addr = detour;

            VirtualProtect(addr, sizeof(void*), oldProt, &oldProt);
        }
    }

    __declspec(noinline) bool InternalCheckBytes(void* base, int ind, const uint8_t* bytes, size_t sz, bool upwards) {
        auto offBase = (uint8_t*)(upwards ? __int64(base) - ind : __int64(base) + ind);
        for (int i = 0; i < sz; i++) {
            if (*(offBase + i) != bytes[i]) return false;
        }
        return true;
    }
}