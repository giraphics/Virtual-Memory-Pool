#pragma once

#include <stdint.h>
#include <stddef.h>

#include "VirtualMemoryUtils.h"

#if GFX_SYSTEM_DEFAULT_ALIGNMENT == 4
    static constexpr uint32_t g_PoolCount                   = 9;
#elif GFX_SYSTEM_DEFAULT_ALIGNMENT == 8
    static constexpr uint32_t g_PoolCount                   = 8;
#else
#error "Invalid system memory alignment"
#endif

struct VMPoolConfig
{
    size_t   poolSize;
    uint32_t poolCapacity;
};

// Demo configuration table, please build of your own as per your renderer need and configure it to UiGfxVirtualMemoryPool
static VMPoolConfig g_VirtualMemoryTable[g_PoolCount] =
{
    /* Block Size       Capacity */
    /*****************************/
#if GFX_SYSTEM_DEFAULT_ALIGNMENT == 4
    { 4,                GFX_MEM_MB(1)  },
#endif
    { 8,                GFX_MEM_MB(1)  },
    { 16,               GFX_MEM_MB(1)  },
    { 32,               GFX_MEM_MB(1)  },
    { 64,               GFX_MEM_MB(1)  },
    { 128,              GFX_MEM_MB(20) },
    { 256,              GFX_MEM_MB(20) },
    { 512,              GFX_MEM_MB(8)  },
    { 1024,             GFX_MEM_MB(2)  },
};
