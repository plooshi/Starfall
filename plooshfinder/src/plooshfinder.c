// plooshfinder
// WIP patchfinder
// Made by Ploosh

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../include/plooshfinder.h"
#include "../include/plooshfinder_sig.h"
#include "../include/formats/pe.h"

void *pf_zero_buf;

struct pf_patch_t pf_construct_patch(void *matches, void *masks, uint32_t count, bool (*callback)(struct pf_patch_t *patch, void *stream)) {
    struct pf_patch_t patch;

    // construct the patch
    patch.matches = matches;
    patch.masks = masks;
    patch.disabled = false;
    patch.count = count;
    patch.callback = callback;

    return patch;
}

struct pf_patchset_t pf_construct_patchset(struct pf_patch_t *patches, uint32_t count, bool (*handler)(void *buf, size_t size, struct pf_patchset_t patchset)) {
    struct pf_patchset_t patchset;

    patchset.patches = patches;
    patchset.count = count;
    patchset.handler = handler;

    return patchset;
}

bool pf_patchset_emit(void *buf, size_t size, struct pf_patchset_t patchset) {
    return patchset.handler(buf, size, patchset);
}

void pf_disable_patch(struct pf_patch_t *patch) {
    patch->disabled = true;
}

uint8_t *pf_find_next(uint8_t *stream, uint32_t count, uint8_t match, uint8_t mask) {
    uint8_t *find_stream = 0;

    for (int i = 0; (uint32_t) i < count; i++) {
        if (pf_maskmatch(stream[i], match, mask)) {
            find_stream = stream + i;
            break;
        }
    }

    return find_stream;
}

uint8_t *pf_find_prev(uint8_t *stream, uint32_t count, uint8_t match, uint8_t mask) {
    uint8_t *find_stream = 0;

    for (int neg_count = -(int)count; count > 0; count--) {
        int ind = neg_count + count;
        if (pf_maskmatch(stream[ind], match, mask)) {
            find_stream = stream + ind;
            break;
        }
    }

    return find_stream;
}

uint8_t* pf_find_next_multiple(uint8_t* stream, uint32_t count, uint8_t* matches, uint8_t* masks, uint32_t mmc) {
    uint8_t* find_stream = 0;

    for (int i = 0; (uint32_t)i < count; i++) {
        bool cont = false;
        for (uint32_t i2 = 0; i2 < mmc; i2++) {
            if (!pf_maskmatch(stream[i + i2], matches[i2], masks[i2])) {
                cont = true;
                break;
            }
        }
        if (cont) continue;

        find_stream = stream + i;
        break;
    }

    return find_stream;
}

uint8_t* pf_find_prev_multiple(uint8_t* stream, uint32_t count, uint8_t *matches, uint8_t *masks, uint32_t mmc) {
    uint8_t* find_stream = 0;

    for (int neg_count = -(int)count; count > 0; count--) {
        int ind = neg_count + count;
        bool cont = false;
        for (uint32_t i = 0; i < mmc; i++) {
            if (!pf_maskmatch(stream[ind], matches[i], masks[i])) {
                cont = true;
                break;
			}
		}
        if (cont) continue;
        find_stream = stream + ind;
        break;
    }

    return find_stream;
}

bool pf_set_zero_buf(struct pf_patch_t *patch, uint8_t *stream) {
    pf_zero_buf = stream;

    pf_disable_patch(patch);
    return true;
}

void *pf_find_zero_buf(void *buf, size_t size, size_t shc_count) {
    pf_zero_buf = NULL;

    uint8_t *matches = (uint8_t *) malloc(shc_count);
    uint8_t *masks = (uint8_t *) malloc(shc_count);

    for (size_t i = 0; i < shc_count; i++) {
        matches[i] = 0;
        masks[i] = 0xff;
    }

    struct pf_patch_t patch = pf_construct_patch(matches, masks, shc_count / sizeof(uint8_t), (bool (*)(struct pf_patch_t *, void *stream)) pf_set_zero_buf);

    struct pf_patch_t patches[] = {
        patch
    };

    struct pf_patchset_t patchset = pf_construct_patchset(patches, sizeof(patches) / sizeof(struct pf_patch_t), (bool (*)(void *, size_t, struct pf_patchset_t)) pf_find_maskmatch);

    pf_patchset_emit(buf, size, patchset);

#ifndef NOLOG
    if (!pf_zero_buf) {
        printf("%s: Unable to find zero buf!\n", __FUNCTION__);
    }
#endif
    return pf_zero_buf;
}