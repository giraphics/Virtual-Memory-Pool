#pragma once

#include "VirtualMemoryPool.h"

class TestVMPool
{
public:
    TestVMPool()
    {
        m_MemoryPool = std::unique_ptr<VirtualMemoryPool>(new VirtualMemoryPool(g_VirtualMemoryTable));
        m_MyInt = static_cast<int*>(m_MemoryPool->AllocateVirtualMemory(sizeof(int)));
        m_MemoryPool->PrintStats();

        m_MemoryPool->FreeVirtualMemory(m_MyInt);
        m_MemoryPool->PrintStats();

        m_MyInt = static_cast<int*>(m_MemoryPool->AllocateVirtualMemory(sizeof(int)));
        m_MemoryPool->PrintStats();

        *m_MyInt = 55;
    }

    ~TestVMPool()
    {
    }

    void Draw()
    {
        int a = *m_MyInt;
        int b = 9;
        m_MemoryPool->PrintStats();
    }

private:
    std::unique_ptr<VirtualMemoryPool> m_MemoryPool = nullptr;
    int* m_MyInt = nullptr;
};

int main()
{
    TestVMPool a;
    a.Draw();

    return 0;
}
