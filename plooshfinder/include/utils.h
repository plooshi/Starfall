#ifndef _UTILS_H
#define _UTILS_H
#include <stdint.h>

#ifdef __cplusplus
#define PF_C extern "C"
#else
#define PF_C
#endif

PF_C uint32_t convert_endianness32(uint32_t val);
#define Cast(T, expr) (T) (expr)

#endif