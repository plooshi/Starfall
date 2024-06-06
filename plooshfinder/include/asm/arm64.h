#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#define PF_C extern "C"
#else
#define PF_C
#endif

PF_C uint32_t arm64_branch(void *caller, void *target, bool link);