#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../include/formats/macho.h"
#include "../../include/formats/elf.h"
#include "../../include/formats/pe.h"
#include "../../include/utils.h"
// right now this is just ptr & virtual address conversion

char *pf_va_to_ptr(char *buf, uint64_t addr) {
    char *ptr = NULL;

    if (macho_check(buf)) {
        ptr = macho_va_to_ptr(buf, addr);
    } else if (elf_check(buf)) {
        ptr = elf_va_to_ptr(buf, addr);
    } else if (pe_check(buf)) {
        ptr = pe_va_to_ptr(buf, addr);
    } else {
        printf("%s: Unknown binary format!\n", __FUNCTION__);
    }

    return ptr;
}

uint64_t pf_ptr_to_va(char *buf, char *ptr) {
    uint64_t va = 0;

    if (macho_check(buf)) {
        va = macho_ptr_to_va(buf, ptr);
    } else if (elf_check(buf)) {
        va = elf_ptr_to_va(buf, ptr);
    } else if (pe_check(buf)) {
        va = pe_ptr_to_va(buf, ptr);
    } else {
        printf("%s: Unknown binary format!\n", __FUNCTION__);
    }

    return va;
}