#ifndef _MULTI_H
#define _MULTI_H
#include <stdint.h>

#ifdef __cplusplus
#define PF_C extern "C"
#else
#define PF_C
#endif

PF_C void *pf_va_to_ptr(void *buf, uint64_t addr);
PF_C uint64_t pf_ptr_to_va(void *buf, void *ptr);

#endif