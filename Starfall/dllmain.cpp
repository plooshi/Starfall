#include "pch.h"
#include "base.h"
#include "request.h"
#include "exit.h"
#include "opts.h"
#include <windows.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")

namespace Starfall {
    void Init() {
        if (Console) {
            AllocConsole();

            FILE* fptr;
            freopen_s(&fptr, "CONOUT$", "w+", stdout);
            SetConsoleTitleA("Starfall - https://github.com/plooshi/Starfall");
        }
       
        const char* imageUrl = "https://media.discordapp.net/attachments/1323527643282411543/1356019387131363559/shutterstock_1607279755-min.png?ex=67eb0af7&is=67e9b977&hm=1aed8ba404fd1408a41260bd1be75adaf8b32bb1b9d0b4a5de95bbb3747292ad&=&format=webp&quality=lossless&width=1376&height=860";
        const char* localPath = "C:\\Windows\\Temp\\background.jpg";

        if (URLDownloadToFileA(NULL, imageUrl, localPath, 0, NULL) == S_OK) {
            
            SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (PVOID)localPath, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
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
        EOSBuf = LoadLibraryA("EOSSDK-Win64-Shipping");

        {
            auto section = pe_get_section((char*)buf, ".text");
            auto rsection = pe_get_section((char*)buf, ".rdata");

            tbuf = (void*)(__int64(buf) + section->virtualAddress);
            tsize = section->virtualSize;

            rbuf = (void*)(__int64(buf) + rsection->virtualAddress);
            rsize = rsection->virtualSize;
        }

        if (EOSBuf) {
            auto section = pe_get_section((char*)EOSBuf, ".text");
            auto rsection = pe_get_section((char*)EOSBuf, ".rdata");

            EOSTextBuf = (void*)(__int64(EOSBuf) + section->virtualAddress);
            EOSTextSize = section->virtualSize;

            EOSRDataBuf = (void*)(__int64(EOSBuf) + rsection->virtualAddress);
            EOSRDataSize = rsection->virtualSize;
        }

        FindProcessRequest();
        if (bHasPushWidget) FindPushWidget();

        return;
    }
}

BOOL APIENTRY DllMain(HMODULE dllBase,
    DWORD  callReason,
    LPVOID lpReserved
)
{
    if (callReason == 1) ManualMapping ? Init() : (void)CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Init, 0, 0, 0);
    return TRUE;
}