#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "../include/plooshfinder.h"
#include "../include/plooshfinder8.h"
#include <Windows.h>

bool pf_maskmatch(uint8_t insn, uint8_t match, uint8_t mask) {
    return (insn & mask) == match;
}

bool pf_find_maskmatch(void *buf, size_t size, struct pf_patchset_t patchset) {
    uint8_t *stream = buf;

    for (uint64_t i = 0; i < size; i++) {
        for (uint32_t p = 0; p < patchset.count; p++) {
            struct pf_patch8_t *patch = (struct pf_patch8_t *) patchset.patches + p;
            if (patch->disabled) continue;

            uint32_t x;
            for (x = 0; x < patch->count; x++) {
                if (!pf_maskmatch(stream[i + x], patch->matches[x], patch->masks[x])) {
                    break;
                }
            }

            if (x == patch->count) {
                if (patch->callback(patch, stream + i)) {
                    uint32_t disabled = 0;
                    for (uint32_t p = 0; p < patchset.count; p++) {
                        struct pf_patch8_t* pt = (struct pf_patch8_t *) patchset.patches + p;
                        if (pt->disabled) {
                            disabled++;
                            continue;
                        }

                        if (patch->callback == pt->callback) {
                            DWORD og;
                            VirtualProtect(pt + offsetof(struct pf_patch8_t, disabled), sizeof(bool), PAGE_READWRITE, &og); // this is only needed bc constexpr lmao
                            pt->disabled = true; // disable patches that have already been found
                            VirtualProtect(pt + offsetof(struct pf_patch8_t, disabled), sizeof(bool), og, &og);
                            disabled++;
                        }
                    }
                    if (disabled == patchset.count) return true; // if all are done, return
                }
            }
        }
    }
    return false;
}

int8_t pf_signextend(int8_t val, uint8_t bits) {
    val = (uint8_t) val << (8 - bits);
    val >>= 8 - bits;

    return val;
}
