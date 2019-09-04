#pragma once

#include <stdint.h>
#include <stddef.h>
#include <map>

#include "VirtualMemory.h"
#include "VirtualMemoryPoolConfig.h"

struct FreeStack
{
    PageAllocation pageAlloc;
    void** addressPtr = nullptr;

    size_t capacity = 0;
    size_t count = 0;
} ;

struct MemoryPage
{
    FreeStack freeStack;
    PageAllocation pageAlloc;
    void* baseAddress    = nullptr;
    void* currentAddress = nullptr;
    size_t elementSize  = 0;
    size_t usedByteSize = 0;
};

struct MemoryPool
{
    MemoryPage pools[g_PoolCount];
};

class VirtualMemoryPool : public VirtualMemory
{
public:
    explicit VirtualMemoryPool(VMPoolConfig* vmPoolConfig);
    virtual ~VirtualMemoryPool();

    void* AllocateVirtualMemory(size_t size);
    void FreeVirtualMemory(void* pointer);

    void PrintStats();

private:
    size_t InUsedMemory();
    size_t TotalMemoryCapacity();

private:
    std::map<size_t, size_t> m_PoolSizeLookupTableIdx;
    VMPoolConfig m_PoolConfig[g_PoolCount];
    MemoryPool m_VMPool;
};
