#include "pch.h"
#include "hooking.h"
#include "base.h"
#include "exit.h"

namespace Starfall {
    int dontExit = 0;
    namespace Hooks {
        void RequestExitWithStatusHook(bool Force, unsigned char Code)
        {
            if (dontExit == 0) {
                TerminateProcess(GetCurrentProcess(), Code);
            }
            else {
                dontExit--;
            }
        }

        void UnsafeEnvironmentPopupHook(wchar_t** unknown1, unsigned __int8 _case, __int64 unknown2, char unknown3)
        {
            dontExit++;
        }
    }

    namespace Callbacks {
        bool RequestExitWithStatusCallback(struct pf_patch_t* patch, void* stream) {
            AsmHook(stream, Hooks::RequestExitWithStatusHook);
            AsmHook((void*)(uint64_t(buf) + 0x221a404), Hooks::RequestExitWithStatusHook);
            return true;
        }

        bool UnsafeEnvironmentPopupCallback(struct pf_patch_t* patch, void* stream) {
            AsmHook(stream, Hooks::UnsafeEnvironmentPopupHook);
            return true;
        }

        bool PushWidgetCallback(struct pf_patch_t* patch, void* stream) {
            FindExit();
            return true;
        }
    }

    namespace Finders {
        void FindExit() {
            constexpr static struct pf_patch_t patch = pf_construct_patch_sig("48 89 5C 24 ? 57 48 83 EC 40 41 B9 ? ? ? ? 0F B6 F9 44 38 0D ? ? ? ? 0F B6 DA 72 24 89 5C 24 30 48 8D 05 ? ? ? ? 89 7C 24 28 4C 8D 05 ? ? ? ? 33 D2 48 89 44 24 ? 33 C9 E8 ? ? ? ?", RequestExitWithStatusCallback);
            constexpr static struct pf_patch_t patch2 = pf_construct_patch_sig("48 8B C4 48 89 58 18 88 50 10 88 48 08 57 48 83 EC 30", RequestExitWithStatusCallback);
            constexpr static struct pf_patch_t patch3 = pf_construct_patch_sig("4C 8B DC 49 89 5B 08 49 89 6B 10 49 89 73 18 49 89 7B 20 41 56 48 83 EC 30 80 3D ? ? ? ? ? 49 8B", RequestExitWithStatusCallback);

            constexpr static struct pf_patch_t patch4 = pf_construct_patch_sig("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 73 F0 49 89 7B E8 48 8B F9 4D 89 63 E0 4D 8B E0 4D 89 6B D8", Ret0Callback); // 17.30
            constexpr static struct pf_patch_t patch5 = pf_construct_patch_sig("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 20 48 8B D9 4D 89 63 E0 4D 89 6B D8", Ret0Callback); // 19.10
            constexpr static struct pf_patch_t patch6 = pf_construct_patch_sig("48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 80 B9 ? ? ? ? ? 48 8B DA 48 8B F1", Ret0Callback); // 28.00
            constexpr static struct pf_patch_t patch7 = pf_construct_patch_sig("40 55 53 56 57 41 54 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? ? 0F B6 ?", Ret0Callback); // 29.00/22.40
            constexpr static struct pf_patch_t patch8 = pf_construct_patch_sig("48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? ? 0F B6 ? 44 88 44 24 ?", Ret0Callback); // 30.00
            

            constexpr static struct pf_patch_t patches2[] = {
                patch,
                patch2,
                patch3,
                patch4,
                patch5,
                patch6,
                patch7,
                patch8
            };

            constexpr static struct pf_patchset_t patchset2 = pf_construct_patchset(patches2, sizeof(patches2) / sizeof(struct pf_patch_t), (bool (*)(void*, size_t, pf_patchset_t))pf_find_maskmatch);

            pf_patchset_emit(tbuf, tsize, patchset2);
        }

        void FindPushWidget()
        {
            constexpr static auto patch = pf_construct_patch_sig("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 30 48 8B E9 49 8B D9 48 8D 0D ? ? ? ? 49 8B F8 48 8B F2 E8 ? ? ? ? 4C 8B CF 48 89 5C 24 ? 4C 8B C6 48 8B D5 48 8B 48 78", PushWidgetCallback);
            constexpr static auto patch2 = pf_construct_patch_sig("48 8B C4 4C 89 40 18 48 89 50 10 48 89 48 08 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 68 B8 48 81 EC ? ? ? ? 65 48 8B 04 25", PushWidgetCallback); // 26.00
            constexpr static auto patch3 = pf_construct_patch_sig("48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 55 41 56 41 57 48 8D 68 A1 48 81 EC ? ? ? ? 65 48 8B 04 25 ? ? ? ? 48 8B F9 B9 ? ? ? ? 49", PushWidgetCallback); // 28.00
            constexpr static auto patch4 = pf_construct_patch_sig("48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 55 41 56 41 57 48 8D 68 A1 48 81 EC ? ? ? ? 65 48 8B 04 25 ? ? ? ? 48 8B F9 B9 ? ? ? ? 48", PushWidgetCallback); // 30.00

            constexpr static struct pf_patch_t patches[] = {
                patch,
                patch2,
                patch3,
                patch4
            };

            constexpr static struct pf_patchset_t patchset = pf_construct_patchset(patches, sizeof(patches) / sizeof(struct pf_patch_t), (bool (*)(void*, size_t, pf_patchset_t))pf_find_maskmatch);

            pf_patchset_emit(tbuf, tsize, patchset);
            //AsmHook((void*)(uint64_t(buf) + 0x221a404), Hooks::RequestExitWithStatusHook);
            //AsmHook((void*)(uint64_t(buf) + 0x221a438), Hooks::RequestExitWithStatusHook);
            //AsmHook((void*)(uint64_t(buf) + 0x48830b0), Hooks::UnsafeEnvironmentPopupHook);
        }
    }
}