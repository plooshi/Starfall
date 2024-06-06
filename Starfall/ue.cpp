#include "pch.h"
#include "base.h"
#include "ue.h"
namespace Starfall {
    template <typename T>
    __forceinline T* Alloc(size_t sz) {
        return (T*)malloc(sz);
    }
}

namespace Unreal {
    FString::FString()
    {
        String = nullptr;
        Length = MaxSize = 0;
    }

    FString::FString(const char* Other)
    {
        if (Other)
        {
            MaxSize = Length = (int)strlen(Other) + 1;

            AllocString();

            size_t ConvertedChars = 0;
            mbstowcs_s(&ConvertedChars, String, Length, Other, _TRUNCATE);
        }
    }

    FString::FString(wchar_t* Other)
    {
        if (Other) {
            MaxSize = Length = (int)wcslen(Other) + 1;

            AllocString();

            __movsb((PBYTE)String, (const PBYTE)Other, Length * sizeof(wchar_t));
        }
    }

    FString::FString(uint32_t len) {
        MaxSize = Length = len + 1;
        AllocString();
    }

    __declspec(noinline) FString FString::operator+(FString other) {
        if (!String || !other.String) return *this;
        auto sLen = Length - 1;
        auto oLen = other.Length - 1;
        FString nStr((uint32_t)(sLen + oLen));
        __movsb((PBYTE)nStr.String, (const PBYTE)String, sLen * sizeof(wchar_t));
        __movsb((PBYTE)nStr.String + sLen * sizeof(wchar_t), (const PBYTE)other.String, oLen * sizeof(wchar_t));
        nStr.String[nStr.Length - 1] = 0;
        return nStr;
    }

    __declspec(noinline) void FString::operator+=(FString other) {
        if (!String || !other.String) return;
        auto sLen = Length - 1;
        auto oLen = other.Length - 1;
        Length = (uint32_t)(sLen + oLen + 1);
        auto os = String;
        AllocString();
        __movsb((PBYTE)String, (const PBYTE)os, sLen * sizeof(wchar_t));
        __movsb((PBYTE)String + sLen * sizeof(wchar_t), (const PBYTE)other.String, oLen * sizeof(wchar_t));
        String[Length - 1] = 0;
        free(os);
    }

    __declspec(noinline) FString FString::substr(size_t off, size_t count) {
        if (count == -1) count = Length - off - 1;
        else if (count > Length) return *this;
        FString nStr((uint32_t)count);

        __movsb((PBYTE) nStr.String, (const PBYTE) (String + off), count * sizeof(wchar_t));
        nStr.String[count] = 0;
        return nStr;
    }

    __declspec(noinline) size_t FString::find(wchar_t c) {
        for (uint32_t i = 0; i < Length; i++) {
            if (String[i] == c) return i;
        }
        return -1;
    }

    size_t FString::find(char c) {
        return find((wchar_t)c);
    }

    size_t FString::find(const wchar_t* c) {
        for (uint32_t i = 0; i < Length; i++) {
            bool found = true;
            for (int x = 0; x < wcslen(c); x++) {
                if (String[i + x] != c[x]) {
                    found = false;
                    break;
                }
            }
            if (found) return i;
        }
        return -1;
    }

    bool FString::contains(wchar_t c) {
        for (uint32_t i = 0; i < Length; i++) {
            if (String[i] == c) return true;
        }
        return false;
    }

    bool FString::contains(const wchar_t* c) {
        for (uint32_t i = 0; i < Length; i++) {
            bool found = true;
            for (int x = 0; x < wcslen(c); x++) {
                if (String[i + x] != c[x]) {
                    found = false;
                    break;
                }
            }
            if (found) return true;
        }
        return false;
    }

    bool FString::starts_with(const wchar_t* c) {
        for (int x = 0; x < wcslen(c); x++) {
            if (String[x] != c[x]) {
                return false;
            }
        }
        return true;
    }

    bool FString::ends_with(const wchar_t* c) {
        auto cLen = wcslen(c);
        auto start = ((size_t) Length - 1) - cLen;
        for (size_t x = 0; x < cLen; x++) {
            if (String[start + x] != c[x]) {
                return false;
            }
        }
        return true;
    }

    size_t FString::find_first_of(char c) {
        return find(c);
    }

    size_t FString::find_first_of(wchar_t c) {
        return find(c);
    }

    wchar_t* FString::c_str() {
        return String;
    }

    FString::operator wchar_t* () {
        return String;
    }

    __declspec(noinline) void FString::Dealloc() {
        free(String);
        String = nullptr;
        MaxSize = Length = 0;
    }

    inline void FString::AllocString() {
        String = Alloc<wchar_t>(Length * sizeof(wchar_t));
    }
}