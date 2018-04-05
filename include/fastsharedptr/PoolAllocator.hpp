
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
#include <fastsharedptr/AllocatorTraits.hpp>

template<typename T>
struct PooledObject {
    T object;
    T* next{nullptr};
};

template<typename T, size_t MaxSize>
class fixed_size {
public:
    
    constexpr static const size_t OBJECT_SIZE = sizeof(PooledObject<T>);
    constexpr static const size_t OBJECTS_PER_BLOCK = MaxSize / OBJECT_SIZE;
    constexpr static const size_t BLOCK_SIZE = OBJECTS_PER_BLOCK * OBJECT_SIZE;
    
    fixed_size() :
        mObjects(reinterpret_cast<PooledObject<T>*>(::operator new(BLOCK_SIZE)))
    {
        memset(mObjects, 0, BLOCK_SIZE);
    }

    ~fixed_size() {
        ::operator delete( reinterpret_cast<void*>(mObjects) );
    }
    
    T* operator[](size_t index) {
        if(index >= OBJECTS_PER_BLOCK) throw std::bad_alloc();
        return &mObjects[index].object;
    }
    
    size_t capacity(){ return OBJECTS_PER_BLOCK; }
    
private:
    
    PooledObject<T>* mObjects;
};

template<typename T, size_t BlockSize>
class dynamic_size {
public:
    constexpr static const size_t OBJECT_SIZE = sizeof(PooledObject<T>);
    constexpr static const size_t OBJECTS_PER_BLOCK = BlockSize/OBJECT_SIZE;
    constexpr static const size_t BLOCK_SIZE = OBJECTS_PER_BLOCK * OBJECT_SIZE;
    
    dynamic_size() : mBlocks(1) {}
    ~dynamic_size(){ mBlocks.clear(); }
    
    T* operator[](size_t index) {
        size_t block = floor(index / OBJECTS_PER_BLOCK);
        if(block == mBlocks.size()){
            mBlocks.emplace_back();
        }else if(block > mBlocks.size()){
            for(int i = 0; i < (block - mBlocks.size()); i++ ){
                mBlocks.emplace_back();
            }
        }
        auto it = mBlocks.begin();
        std::advance(it, block);
        return (*it)[index % OBJECTS_PER_BLOCK];
    }
    
    size_t capacity(){ return mBlocks.size() * OBJECTS_PER_BLOCK; }
    
private:
    std::list<fixed_size<T,BlockSize>> mBlocks;
};

template<typename T, class StoragePolicy>
class pool_allocator
{
public:
    
    ALLOCATOR_TRAITS(T)
    
    template<typename U>
    struct rebind
    {
        typedef pool_allocator<U,StoragePolicy> other;
    };
    
    // Default Constructor
    pool_allocator() = default;
    
    // Copy Constructor
    template<typename U>
    pool_allocator(pool_allocator<U,StoragePolicy> const& other){
        //static_assert(sizeof(U) == sizeof(T), "allocated sizes must be equal");
        mStorage = other.mStorage;
    }
    
    // Allocate memory from freestore
    pointer allocate(size_type count = 1, const_pointer hint = 0)
    {
        pointer ret;
        if(count != 1){throw std::bad_alloc();}
        if(mFreeStore){
            ret = mFreeStore;
            auto po = reinterpret_cast<PooledObject<T>*>(mFreeStore);
            mFreeStore = po->next;
        }else{
            ret = mStorage[mLast++];
        }
        return ret;
    }
    
    // place freed memory on the freestore
    void deallocate(pointer ptr, size_type count = 1)
    {
        if(count != 1){throw std::exception();}
        auto po = reinterpret_cast<PooledObject<T>*>(ptr);
        po->next = mFreeStore;
        mFreeStore = ptr;
    }
    
    // Max number of objects that can be allocated in one call
    size_type max_size(void) const {return StoragePolicy::BLOCK_SIZE;}
    
    size_t capacity(){ return mStorage.capacity(); }
    
private:
    pointer mFreeStore{nullptr};
    size_t mLast{0};
    StoragePolicy mStorage;
    
    template<typename, class > friend class pool_allocator;
};


