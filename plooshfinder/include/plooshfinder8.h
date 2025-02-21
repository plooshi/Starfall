#ifndef _PLOOSHFINDER8_H
#define _PLOOSHFINDER8_H
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
#define PF_C extern "C"
#else
#define PF_C
#endif

#pragma pack(push, 0x1)
struct pf_patch8_t {
    uint8_t *matches;
    uint8_t *masks;
    bool disabled;
    uint32_t count;
    bool (*callback)(struct pf_patch8_t *patch, void *stream);
};

struct pf_patchset8_t {
    struct pf_patch8_t *patches;
    uint32_t count;
};
#pragma pack(pop)

// patch utils
PF_C bool pf_maskmatch(uint8_t insn, uint8_t match, uint8_t mask);
PF_C bool pf_find_maskmatch(void *buf, size_t size, struct pf_patchset_t patchset);

// utils for finding
PF_C int8_t pf_signextend(int8_t val, uint8_t bits);

#endif
