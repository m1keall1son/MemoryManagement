
#pragma once

//
//  PoolAllocator.h
//  PoolAllocator
//
//  Created by Mike Allison on 2/26/17.
//  Copyright (c) 2017 Mike Allison. All rights reserved.
//

#include <exception>
#include <list>
#include <iostream>
#include "Heap.hpp"
#include "FreeStore.hpp"

template<typename T, template<size_t,size_t> typename StorageType, size_t StorageSize>
class FreeStoreAllocator
{
public:
    
    ALLOCATOR_TRAITS(T)
    
    template<typename U>
    struct rebind
    {
        typedef FreeStoreAllocator<U,StorageType,StorageSize> other;
    };
    
    // Default Constructor
    FreeStoreAllocator() = default;
    
    // Copy Constructor
    template<typename U>
    FreeStoreAllocator(FreeStoreAllocator<U,StorageType,StorageSize> const& other){}
    
    // Allocate memory from freestore
    pointer allocate(size_type count = 1, const_pointer hint = 0)
    {
        if(count == 1){
            return static_cast<pointer>(FreeStore<sizeof(T),StorageType<sizeof(T),StorageSize>>::get()->allocate());
        }else{
            return static_cast<pointer>(Heap<sizeof(T)>::get()->allocate(count));
        }
    }
    
    // place freed memory on the freestore
    void deallocate(pointer ptr, size_type count = 1)
    {
        if(count == 1){
             FreeStore<sizeof(T),StorageType<sizeof(T),StorageSize>>::get()->deallocate(ptr);
        }else{
            Heap<sizeof(T)>::get()->deallocate(ptr);
        }
    }
    
    // Max number of objects that can be allocated in one call
    size_type max_size(void) const {return StorageType<sizeof(T),StorageSize>::BLOCK_SIZE;}
    size_t capacity(){ return FreeStore<sizeof(T),StorageType<sizeof(T),StorageSize>>::get()->capacity(); }
    
};


