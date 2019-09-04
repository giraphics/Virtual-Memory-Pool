#include "VirtualMemoryPool.h"

#include <assert.h>
#include <inttypes.h>

VirtualMemoryPool::VirtualMemoryPool(VMPoolConfig* vmPoolConfig)
{
    if (!vmPoolConfig)
    {
        LOG_ERROR("Unable to find the virtaul memory pool configuration information.");
        assert(false);
    }

    for (uint32_t poolIdx = 0; poolIdx < g_PoolCount; ++poolIdx)
    {
        m_PoolConfig[poolIdx].poolSize = vmPoolConfig[poolIdx].poolSize;
        m_PoolConfig[poolIdx].poolCapacity = vmPoolConfig[poolIdx].poolCapacity;
    }

    for (uint32_t poolIdx = 0; poolIdx < g_PoolCount; ++poolIdx)
    {
        m_PoolSizeLookupTableIdx[m_PoolConfig[poolIdx].poolSize] = poolIdx;
    }

    for (uint32_t poolIdx = 0; poolIdx < g_PoolCount; ++poolIdx)
    {
        MemoryPage* memPool = &m_VMPool.pools[poolIdx];

        size_t elementSize   = m_PoolConfig[poolIdx].poolSize;
        size_t poolCapacity  = m_PoolConfig[poolIdx].poolCapacity;
        size_t stackCapacity = poolCapacity / elementSize;
        size_t stackByteSize = stackCapacity * sizeof(void*);

        memPool->elementSize        = elementSize;
        memPool->usedByteSize       = 0;
        memPool->freeStack.count    = 0;
        memPool->freeStack.capacity = stackCapacity;

        bool success = VmAllocate(poolCapacity, &memPool->pageAlloc);
        if (!success)
        {
            LOG_ERROR("Failed to allocate virtual memory for pool of size %zu", elementSize);
            assert(success);
        }

        success = VmAllocate(stackByteSize, &memPool->freeStack.pageAlloc);
        if (!success)
        {
            LOG_ERROR("Failed to allocate virtual memory for pool's free stack of size %zu", elementSize);
            assert(success);
        }

        memPool->freeStack.addressPtr = (void**)memPool->freeStack.pageAlloc.baseAddress;
        memPool->baseAddress = memPool->pageAlloc.baseAddress;
        memPool->currentAddress = memPool->baseAddress;
    }
}

VirtualMemoryPool::~VirtualMemoryPool()
{
}

void* VirtualMemoryPool::AllocateVirtualMemory(size_t size)
{
    if (size < m_PoolConfig[0].poolSize)
    {
        size = m_PoolConfig[0].poolSize;
    }

    GFX_NEXT_POWER_OF_2(size);
    if (size <= m_PoolConfig[g_PoolCount - 1].poolSize)
    {
        uint32_t poolIndex = m_PoolSizeLookupTableIdx[size];
        MemoryPage* pPool = &m_VMPool.pools[poolIndex];
        if (pPool->freeStack.count > 0)
        {
            return pPool->freeStack.addressPtr[--pPool->freeStack.count];
        }

        void* pNewAddress = pPool->currentAddress;
        void* pNextAddress = GFX_ADVANCE_POINTER_BY_OFFSET(pNewAddress, size);
        if (GFX_IS_VALID_RANGE(pNextAddress, pPool->baseAddress, GFX_ADVANCE_POINTER_BY_OFFSET(pPool->baseAddress, pPool->pageAlloc.size)))
        {
            pPool->currentAddress = pNextAddress;
            pPool->usedByteSize += size;
#if defined(_DEBUG)
            memset(pNewAddress, 0xAA, size);
#endif
            return pNewAddress;
        }
    }

    LOG_ERROR("Attempting an allocation of size: %ld is not supported by the virtual memory pool. Valid memory range is %ld - %ld", size, m_PoolConfig[0].poolSize, m_PoolConfig[g_PoolCount - 1].poolSize);

    return NULL;
}

void VirtualMemoryPool::FreeVirtualMemory(void* pointer)
{
    for (uint32_t index = 0; index < g_PoolCount; ++index)
    {
        MemoryPage* pPool = &m_VMPool.pools[index];
        void* pHead = pPool->baseAddress;
        if (GFX_IS_VALID_RANGE(pointer, pHead, GFX_ADVANCE_POINTER_BY_OFFSET(pHead, pPool->pageAlloc.size)))
        {
            pPool->freeStack.addressPtr[pPool->freeStack.count++] = pointer;
            pPool->usedByteSize -= pPool->elementSize;
#if defined(_DEBUG)
            memset(p, 0xDD, pPool->elementSize);
#endif
            return;
        }
    }

    LOG_ERROR("The memory you are trying to free doesn't belong to any of the pools.");
    assert(false);
}

size_t VirtualMemoryPool::InUsedMemory()
{
    size_t size = 0;
    for (uint32_t index = 0; index < g_PoolCount; ++index)
    {
        size += m_VMPool.pools[index].usedByteSize;
    }

    return size;
}

size_t VirtualMemoryPool::TotalMemoryCapacity()
{
    size_t size = 0;
    for (uint32_t poolIdx = 0; poolIdx < g_PoolCount; ++poolIdx)
    {
        size +=  m_PoolConfig[poolIdx].poolCapacity;
    }

    return size;
}

void VirtualMemoryPool::PrintStats()
{
    size_t minCapacity = GFX_MEM_MB(99);
    for (uint32_t poolIdx = 0; poolIdx < g_PoolCount; ++poolIdx)
    {
        if (minCapacity > m_PoolConfig[poolIdx].poolCapacity)
        {
            minCapacity = m_PoolConfig[poolIdx].poolCapacity;
        }
    }

    for (uint32_t poolIdx = 0; poolIdx < g_PoolCount; ++poolIdx)
    {
        float memoryUsage = static_cast<float>(m_VMPool.pools[poolIdx].usedByteSize) / static_cast<float>(m_PoolConfig[poolIdx].poolCapacity) * 100.0f;
        float relativePoolWeight = static_cast<float>(m_PoolConfig[poolIdx].poolCapacity) / static_cast<float>(minCapacity);
        LOG_INFO("Pool[%d] Usage: %f %%, Relative Pool Weight: %f", poolIdx, memoryUsage, relativePoolWeight);
    }

    LOG_INFO("VM Used Size: %d", InUsedMemory());

    LOG_INFO("VM Total Capacity Size: %d", TotalMemoryCapacity());
}

