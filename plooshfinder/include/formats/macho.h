#ifndef _MACHO_H
#define _MACHO_H
#include <stdbool.h>
#include <stdint.h>
#include "defs/macho_defs.h"

#ifdef __cplusplus
#define PF_C extern "C"
#else
#define PF_C
#endif

PF_C uint32_t macho_get_magic(char *buf);
PF_C bool macho_check(char *buf);
PF_C char *macho_find_arch(char *buf, uint32_t arch);
PF_C uint32_t macho_get_platform(char *buf);
PF_C struct segment_command_64 *macho_get_segment(char *buf, char *name);
PF_C struct section_64 *macho_get_section(char *buf, struct segment_command_64 *segment, char *name);
PF_C struct section_64 *macho_get_last_section(struct segment_command_64 *segment);
PF_C struct section_64 *macho_find_section(char *buf, char *segment_name, char *section_name);
PF_C struct fileset_entry_command *macho_get_fileset(char *buf, char *name);
PF_C struct segment_command_64 *macho_get_segment_for_va(char *buf, uint64_t addr);
PF_C struct section_64 *macho_get_section_for_va(struct segment_command_64 *segment, uint64_t addr);
PF_C struct section_64 *macho_find_section_for_va(char *buf, uint64_t addr);
PF_C char *macho_va_to_ptr(char *buf, uint64_t addr);
PF_C struct segment_command_64 *macho_get_segment_for_ptr(char *buf, char *ptr);
PF_C struct section_64 *macho_get_section_for_ptr(struct segment_command_64 *segment, char *buf, char *ptr);
PF_C struct section_64 *macho_find_section_for_ptr(char *buf, char *ptr);
PF_C uint64_t macho_ptr_to_va(char *buf, char *ptr);
PF_C struct nlist_64 *macho_find_symbol(char *buf, char *name);
PF_C uint64_t macho_get_symbol_size(struct nlist_64 *symbol);
PF_C uint64_t macho_parse_plist_integer(char *key);
PF_C struct mach_header_64 *macho_parse_prelink_info(char *buf, struct section_64 *kmod_info, char *bundle_name);
PF_C uint64_t macho_xnu_untag_va(uint64_t addr);
PF_C struct mach_header_64 *macho_parse_kmod_info(char *buf, struct section_64 *kmod_info, struct section_64 *kmod_start, char *bundle_name);
PF_C struct mach_header_64 *macho_find_kext(char *buf, char *name);
PF_C void macho_run_each_kext(char *buf, void (*function)(char *real_buf, char *kextbuf, uint64_t kext_size));
PF_C char *fileset_va_to_ptr(char *buf, char *kext, uint64_t addr);
PF_C struct segment_command_64 *fileset_get_segment_for_ptr(char *buf, char *kext, char *ptr);
PF_C struct section_64 *fileset_find_section_for_ptr(char *buf, char *kext, char *ptr);
PF_C uint64_t fileset_ptr_to_va(char *buf, char *kext, char *ptr);
PF_C struct nlist_64 *fileset_find_symbol(char *buf, char *kext, char *name);

#endif