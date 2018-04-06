//
//  IAllocator.h
//  MemoryManagement
//
//  Created by Michael Allison on 4/6/18.
//

#pragma once

#include <stdint.h>

template<size_t Size>
struct max_allocations
{
    enum{value = static_cast<std::size_t>(-1) / Size};
};

class IAllocator {
public:
    virtual void * allocate(size_t) = 0;
    virtual void deallocate(void*) = 0;
    virtual size_t capacity()  = 0;
};
