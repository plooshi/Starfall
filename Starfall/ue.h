#pragma once
#include "pch.h"

namespace Unreal {
    class FString
    {
    public:
        wchar_t* String;
        uint32_t Length;
        uint32_t MaxSize;
        inline static const size_t npos = -1;
        FString();

        FString(const char* Other);

        FString(wchar_t* Other);

        consteval FString(const wchar_t* Other) {
            if (Other) {
                MaxSize = Length = (int)std::wstring_view(Other).size() + 1;
                String = (wchar_t*)Other;
            }
        }


        consteval FString(const wchar_t* Other, size_t len) {
            if (Other) {
                MaxSize = Length = (uint32_t) len;
                String = (wchar_t*)Other;
            }
        }

        FString(uint32_t len);

        FString operator+(FString other);

        void operator+=(FString other);

        FString substr(size_t off, size_t count = -1);

        size_t find(wchar_t c);

        size_t find(char c);

        size_t find(const wchar_t* c);

        bool contains(wchar_t c);

        bool contains(const wchar_t* c);

        bool starts_with(const wchar_t* c);

        bool ends_with(const wchar_t* c);

        size_t find_first_of(char c);

        size_t find_first_of(wchar_t c);

        wchar_t* c_str();

        operator wchar_t* ();

        void Dealloc();
    private:
        inline void AllocString();
    };

    class FStringUtil {
    public:
        consteval static size_t find_const(const FString s, const wchar_t c) {
            for (uint32_t i = 0; i < s.Length; i++) {
                if (s.String[i] == c) return i;
            }
            return -1;
        }

        template<FString s, size_t off, size_t count = -1>
        consteval static std::array<wchar_t, count == -1 ? s.Length - off : count + 1> substr() {
            std::array<wchar_t, count == -1 ? s.Length - off : count + 1> arr;

            for (size_t i = 0; i < (count == -1 ? (s.Length - off) : count + 1); i++) {
                arr[i] = (s.String + off)[i];
            }
            arr[(count == -1 ? s.Length - off - 1 : count)] = 0;
            return arr;
        }
    };

    class CPPArrayUtil {
    public:
        template<size_t S, std::array<wchar_t, S> s, size_t off, size_t count = -1>
        consteval static std::array<wchar_t, count == -1 ? S - off : count + 1> substr() {
            if (off == FString::npos) return {};
            std::array<wchar_t, count == -1 ? S - off : count + 1> arr;

            for (size_t i = 0; i < (count == -1 ? (S - off) : count + 1); i++) {
                arr[i] = (s.data() + off)[i];
            }
            arr[(count == -1 ? S - off - 1 : count)] = 0;
            return arr;
        }


        template<size_t S, std::array<wchar_t, S> a>
        consteval static size_t find(const wchar_t c) {
            for (uint32_t i = 0; i < a.size(); i++) {
                if (a[i] == c) return i;
            }
            return -1;
        }
    };
}
using namespace Unreal;