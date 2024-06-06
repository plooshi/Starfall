#pragma once
#include "pch.h"


namespace Starfall {
    namespace Hooking {
        __forceinline void AsmHook(void* ptr, void* detour) {
            if (!ptr || !detour) return;

            uint8_t data[] = {
                0x41, 0x52, // push r10
                0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov r10, detour
                0x41, 0xFF, 0xD2, // call r10
                0x41, 0x5A, // pop r10
                0xC3 // ret
            };

            DWORD oldProt;
            VirtualProtect(ptr, sizeof(data), PAGE_EXECUTE_READWRITE, &oldProt);

            memcpy(ptr, data, sizeof(data));
            *(uint64_t*)(__int64(ptr) + 4) = (uint64_t)detour;

            VirtualProtect(ptr, sizeof(data), oldProt, &oldProt);
        }

        __forceinline void AsmHook(uint64_t ptr, void* detour);

        bool Ret0Callback(struct pf_patch_t* patch, void* stream);

        void VTHook(void** addr, void* detour, void** orig = nullptr);
    }
    using namespace Hooking;

    bool InternalCheckBytes(void* base, int ind, const uint8_t* bytes, size_t sz, bool upwards = false);
    template <uint8_t... Data>
    class CheckBytes {
    public:
        constexpr static uint8_t bytes[sizeof...(Data)] = { Data... };
        void* Base;
        int Ind;
        bool Upwards;

        CheckBytes(void* base, int ind, bool upwards = false) {
            Base = base;
            Ind = ind;
            Upwards = upwards;
		}

        operator bool() {
            return InternalCheckBytes(Base, Ind, bytes, sizeof...(Data), Upwards);
        }
    };
}

//#define CheckBytes(base, ind, bytes, upwards) CheckBytesInternal(base, ind, bytes.data(), bytes.size(), upwards)