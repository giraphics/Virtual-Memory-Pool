#pragma once

#include <stddef.h>

struct PageAllocation
{
    void* baseAddress = nullptr;
    size_t size = 0;
};

class VirtualMemory
{
public:
    VirtualMemory();
    virtual ~VirtualMemory();

    bool VmAllocate(size_t size, PageAllocation* allocationInfo);
    bool VmFree(const PageAllocation* allocationInfo);
    size_t VmSize();
};
