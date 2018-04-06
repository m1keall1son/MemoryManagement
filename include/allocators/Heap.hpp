//
//  Heap.hpp
//  MemoryManagement
//
//  Created by Michael Allison on 4/6/18.
//

#pragma once

#include <stdint.h>
#include <memory>
#include "IAllocator.h"

template <size_t Size>
class Heap : public IAllocator {
public:
    
    static Heap* get(){
        if(!sHeap){
            sHeap.reset(new Heap);
        }
        return sHeap.get();
    }
    
    void* allocate(size_t count)override {
        return ::operator new(count * Size, ::std::nothrow);
    }
    
    void deallocate(void* ptr)override{
        ::operator delete(ptr);
    }
    
    size_t capacity() override { return max_allocations<Size>::value; }
    
private:
    Heap() = default;
    static std::unique_ptr<Heap> sHeap;
};

template <size_t Size>
std::unique_ptr<Heap<Size>> Heap<Size>::sHeap = nullptr;

