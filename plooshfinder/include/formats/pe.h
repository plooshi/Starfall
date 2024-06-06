#ifndef _PE_H
#define _PE_H
#include <stdbool.h>
#include <stdint.h>
#include "defs/pe_defs.h"

#ifdef __cplusplus
#define PF_C extern "C"
#else
#define PF_C
#endif

PF_C bool dos_check(char *buf);
PF_C struct COFF_Header *get_pe_header(char *buf);
PF_C bool pe_check(char *buf);
PF_C bool is_pe(char *buf);
PF_C struct PE64_Optional_Header *get_pe_opt_header(char *buf);
PF_C struct Symbol_Header *pe_get_symtab(char *buf);
PF_C char *pe_get_strtab(char *buf);
PF_C struct Section_Header *pe_get_section(char *buf, const char *name);
PF_C char *pe_va_to_ptr(char *buf, uint64_t addr);
PF_C uint64_t pe_ptr_to_va(char *buf, char *ptr);
PF_C struct Symbol_Header *pe_find_symbol(char *buf, char *name);

#endif