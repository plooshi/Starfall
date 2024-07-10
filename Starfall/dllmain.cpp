#include "pch.h"
#include "base.h"
#include "request.h"
#include "exit.h"
#include "opts.h"

namespace Starfall {
    void Init() {
        if (Console) {
            AllocConsole();

            FILE* fptr;
            freopen_s(&fptr, "CONOUT$", "w+", stdout);
            SetConsoleTitleA("Starfall - https://github.com/ParadiseFN/Starfall");
        }

        if (UseBackendParam) {
            FString cmd = GetCommandLineW();
            auto pos = cmd.find(L"-backend=");
            if (pos != std::wstring::npos) {
                backend = cmd.substr(pos + 9);
            }
            else {
                backend = Backend;
            }
        }

        buf = *(void**)(__readgsqword(0x60) + 0x10);

        auto section = pe_get_section((char*)buf, ".text");
        auto rsection = pe_get_section((char*)buf, ".rdata");

        tbuf = (void*)(__int64(buf) + section->virtualAddress);
        tsize = section->virtualSize;

        rbuf = (void*)(__int64(buf) + rsection->virtualAddress);
        rsize = rsection->virtualSize;

        FindProcessRequest();

        return;
    }
}

BOOL APIENTRY DllMain( HMODULE dllBase,
                       DWORD  callReason,
                       LPVOID lpReserved
                     )
{
    if (callReason == 1) ManualMapping ? Init() : (void) CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Init, 0, 0, 0);
    return TRUE;
}

