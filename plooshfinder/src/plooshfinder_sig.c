#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "../include/plooshfinder.h"
#include "../include/plooshfinder8.h"
#include <stdio.h>

struct pf_patch_t pf_construct_patch_sig(const char *sig, bool (*callback)(struct pf_patch_t *patch, void *stream)) {
    struct pf_patch_t patch;
    char *temp_sig = malloc(strlen(sig) + 1);
    strcpy_s(temp_sig, strlen(sig) + 1, sig);
    void *orig_ts = temp_sig;
    uint8_t temp_match, temp_mask;
    char *part;
    uint32_t count = 0;

    part = strtok(temp_sig, " ");
    while (part != NULL) {
        count++;
        part = strtok(NULL, " ");
    }

    strcpy_s((char *) orig_ts, strlen(sig) + 1, sig);
    temp_sig = orig_ts;
    uint8_t *matches = malloc(count);
    uint8_t *masks = malloc(count);
    uint32_t index = 0;
    
    part = strtok(temp_sig, " ");
    while (part != NULL) {
        size_t len = strlen(part);
        temp_match = 0x0;
        temp_mask = 0x0;

        for (size_t i = 0; i < len; i++) {
            if (part[i] != '?') {
                char hc[2] = {
                    part[i],
                    0
                };
                uint8_t hex = (uint8_t) strtoul(hc, NULL, 16);
                uint8_t shift = (uint8_t) ((len - i - 1) * 4);

                temp_match |= hex << shift;
                temp_mask |= 0xf << shift;
            }
        }

        matches[index] = temp_match;
        masks[index] = temp_mask;
        index++;
        part = strtok(NULL, " ");
    }
    free(orig_ts);

    // construct the patch
    patch.matches = matches;
    patch.masks = masks;
    patch.disabled = false;
    patch.count = count;
    patch.callback = callback;

    return patch;
}

