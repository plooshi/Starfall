#ifndef _ELF_H
#define _ELF_H
#include <stdbool.h>
#include <stdint.h>
#include "defs/elf_defs.h"

#ifdef __cplusplus
#define PF_C extern "C"
#else
#define PF_C
#endif

PF_C bool elf_check(char *buf);
PF_C bool is_elf(char *buf);
PF_C struct elf_sheader_64 *elf_get_section(char *buf, char *name);
PF_C char *elf_va_to_ptr(char *buf, uint64_t addr);
PF_C uint64_t elf_ptr_to_va(char *buf, char *ptr);
PF_C struct elf_symbol_64 *elf_find_symbol_stype(char *buf, char *name, uint32_t type);
PF_C struct elf_symbol_64 *elf_find_symbol(char *buf, char *name);

#endif