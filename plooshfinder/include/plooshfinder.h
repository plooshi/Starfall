#ifndef _PLOOSHFINDER_H
#define _PLOOSHFINDER_H
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef _DEBUG
#pragma comment(lib, "../x64/Release/plooshfinder.lib")
#else 
#pragma comment(lib, "../x64/Debug/plooshfinder.lib")
#endif

#ifdef __cplusplus
#define PF_C extern "C"
#else
#define PF_C
#endif

#pragma pack(push, 0x1)
struct pf_patch_t {
    void *matches;
    void *masks;
    bool disabled;
    uint32_t count;
    bool (*callback)(struct pf_patch_t *patch, void *stream);
};

struct pf_patchset_t {
    struct pf_patch_t *patches;
    uint32_t count;
};
#pragma pack(pop)

// patch utils
#ifdef __cplusplus
constexpr pf_patch_t pf_construct_patch(void* matches, void* masks, uint32_t count, bool (*callback)(struct pf_patch_t* patch, void* stream)) {
    struct pf_patch_t patch;

    // construct the patch
    patch.matches = matches;
    patch.masks = masks;
    patch.disabled = false;
    patch.count = count;
    patch.callback = callback;

    return patch;
}

constexpr pf_patch_t pf_construct_patch_dynmatch(void* masks, uint32_t count, bool (*callback)(struct pf_patch_t* patch, void* stream)) {
    struct pf_patch_t patch;

    // construct the patch
    patch.matches = nullptr;
    patch.masks = masks;
    patch.disabled = false;
    patch.count = count;
    patch.callback = callback;

    return patch;
}

constexpr struct pf_patchset_t pf_construct_patchset(struct pf_patch_t* patches, uint32_t count) {
    struct pf_patchset_t patchset;

    patchset.patches = patches;
    patchset.count = count;

    return patchset;
}

__forceinline constexpr struct pf_patchset_t pf_construct_patchset(const struct pf_patch_t* patches, uint32_t count) {
    return pf_construct_patchset((struct pf_patch_t*)patches, count);
}
#else
PF_C struct pf_patch_t pf_construct_patch(void *matches, void *masks, uint32_t count, bool (*callback)(struct pf_patch_t *patch, void *stream));
PF_C struct pf_patchset_t pf_construct_patchset(struct pf_patch_t* patches, uint32_t count);
#endif
PF_C bool pf_patchset_emit(void *buf, size_t size, struct pf_patchset_t patchset);
PF_C void pf_disable_patch(struct pf_patch_t *patch);

// utils for finding
PF_C uint8_t *pf_find_next(uint8_t *stream, uint32_t count, uint8_t match, uint8_t mask);
PF_C uint8_t *pf_find_prev(uint8_t *stream, uint32_t count, uint8_t match, uint8_t mask);
PF_C uint8_t* pf_find_next_multiple(uint8_t* stream, uint32_t count, uint8_t* matches, uint8_t* masks, uint32_t mmc);
PF_C uint8_t* pf_find_prev_multiple(uint8_t* stream, uint32_t count, uint8_t* matches, uint8_t* masks, uint32_t mmc);

#endif
