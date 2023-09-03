#ifndef H_TYPES
#define H_TYPES
#include <stdint.h>

#define SAVE_DBGLOG

// generic int types
typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;
typedef enum { false, true } bool;

#include "constants.h"
#include "structs.h"

#endif
