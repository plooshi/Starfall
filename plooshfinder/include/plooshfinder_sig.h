#ifndef _PLOOSHFINDER_SIG_H
#define _PLOOSHFINDER_SIG_H
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "plooshfinder8.h"
#include "plooshfinder.h"

#ifdef __cplusplus
#define PF_C extern "C"
#else
#define PF_C
#endif

#ifdef __cplusplus
// compile-time processed implementation of sig patch
constexpr int PatternCount(std::string_view s) {
    int c = 0;
    for (int i = 0; i < s.size(); i++) {
        if (s[i] == ' ') c++;
    }
    return c + 1; // last i think
}

constexpr uint32_t parsePatternPart(std::string_view s) {
    uint32_t val = 0;
    for (int i = 0; i < s.size(); i++) {
        uint8_t byte = s[i];
        if (byte >= '0' && byte <= '9') byte = byte - '0';
        else if (byte >= 'a' && byte <= 'f') byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <= 'F') byte = byte - 'A' + 10;
        else if (byte == '?') byte = 0;
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}

constexpr uint32_t parsePatternMask(std::string_view s) {
    uint32_t val = 0;
    for (int i = 0; i < s.size(); i++) {
        uint8_t byte = s[i];
        if (byte >= '0' && byte <= '9' || byte >= 'a' && byte <= 'f' || byte >= 'A' && byte <= 'F') byte = 0xf;
        else if (byte == '?') byte = 0x0;
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}

#define pf_construct_patch_sig(sig, callback) ConstexprPatch<sig, callback, ([&]() consteval { \
    constexpr auto st = std::string_view(sig); \
    constexpr auto arrsz = PatternCount(st); \
    std::array<uint8_t, arrsz> matches = { 0 }; \
    size_t cInd = 0; \
    for (int i = 0; i < arrsz; i++) { \
        auto part = st.substr(cInd, st.find_first_of(' ', cInd) == std::string_view::npos ? st.size() - cInd : (st.find_first_of(' ', cInd) + 1) - cInd - 1); \
        matches[i] = parsePatternPart(part); \
        cInd = st.find_first_of(' ', cInd) + 1; \
    } \
    return matches; \
})(), ([&]() consteval { \
    constexpr auto st = std::string_view(sig); \
    constexpr auto arrsz = PatternCount(st); \
    std::array<uint8_t, arrsz> masks = { 0 }; \
    size_t cInd = 0; \
        for (int i = 0; i < arrsz; i++) { \
            auto part = st.substr(cInd, st.find_first_of(' ', cInd) == std::string_view::npos ? st.size() - cInd : (st.find_first_of(' ', cInd) + 1) - cInd - 1); \
            masks[i] = parsePatternMask(part); \
            cInd = st.find_first_of(' ', cInd) + 1; \
        } \
        return masks; \
})()>().Create()
#else
PF_C struct pf_patch_t pf_construct_patch_sig(const char *sig, bool (*callback)(struct pf_patch_t *patch, void *stream));
#endif
#endif
