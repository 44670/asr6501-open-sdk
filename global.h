#pragma once
#define ASR6501
#define PSOC_4100S

#include <stdint.h>
#include <string.h>

#define FORCE_INLINE inline __attribute__((always_inline))

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef volatile uint32_t vu32;
typedef u8 bool;
#define true 1
#define false 0

#define MMIO32(x) (*(vu32*)(x))

#include "config.h"

#include "psoc4100s.h"