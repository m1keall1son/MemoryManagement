//
//  FreeStore.hpp
//  MemoryManagement
//
//  Created by Michael Allison on 4/6/18.
//

#pragma once

#include <cstring>
#include <stdint.h>
#include <memory>
#include "IAllocator.h"

struct InBytes {};
struct InNumObjects {};

template<size_t Size, size_t MaxSize>
class FixedSizeStorage {
public:
    
    constexpr static const size_t OBJECT_SIZE = ((Size + sizeof(void *)-1) / sizeof(void *)) * sizeof(void *);
    constexpr static const size_t OBJECTS_PER_BLOCK = MaxSize / OBJECT_SIZE;
    constexpr static const size_t BLOCK_SIZE = OBJECTS_PER_BLOCK * OBJECT_SIZE;
    
    FixedSizeStorage() :
    mObjects(::operator new(BLOCK_SIZE))
    {
        std::memset(mObjects, 0, BLOCK_SIZE);
    }
    
    ~FixedSizeStorage() {
        ::operator delete( mObjects );
    }
    
    void* operator[](size_t index) {
        if(index >= OBJECTS_PER_BLOCK) throw std::bad_alloc();
        char * head = reinterpret_cast<char*>(mObjects);
        return reinterpret_cast<void*>(head + (index*OBJECT_SIZE));
    }
    
    size_t capacity(){ return OBJECTS_PER_BLOCK; }
    size_t max_size(){ return BLOCK_SIZE; }

private:
   void* mObjects;
};

template<size_t Size, size_t BlockSize>
class BlockListStorage {
public:
    
    constexpr static const size_t OBJECT_SIZE = ((Size + sizeof(void *)-1) / sizeof(void *)) * sizeof(void *);
    constexpr static const size_t OBJECTS_PER_BLOCK = BlockSize/OBJECT_SIZE;
    constexpr static const size_t BLOCK_SIZE = OBJECTS_PER_BLOCK * OBJECT_SIZE;
    
    BlockListStorage() : mBlocks(1) {}
    ~BlockListStorage(){ mBlocks.clear(); }
    
    void* operator[](size_t index) {
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
    size_t max_size(){ return mBlocks.size() * BLOCK_SIZE; }
    
private:
    std::list<FixedSizeStorage<Size,BlockSize>> mBlocks;
};

template <size_t Size, typename StorageType>
class FreeStore : public IAllocator{
public:
    
    static FreeStore* get(){
        if(!sFreeStore){
            sFreeStore.reset(new FreeStore);
        }
        return sFreeStore.get();
    }
    
    void* allocate(size_t count = 1)override {
        void* ret;
        if(mFreeStore){
            ret = mFreeStore;
            auto next = *reinterpret_cast<void**>(mFreeStore);
            mFreeStore = next;
        }else{
            ret = mStorage[mLast++];
        }
        std::cout << "allocating size of: " << Size << std::endl;
        return ret;
    }
    
    void deallocate(void* ptr)override{
        *reinterpret_cast<void**>(ptr) = mFreeStore;
        mFreeStore = ptr;
    }
    
    size_t capacity() override { return mStorage.capacity(); }
    size_t max_size(){ return mStorage.max_size(); }

private:
    void* mFreeStore{nullptr};
    size_t mLast{0};
    StorageType mStorage;
    FreeStore() = default;
    static std::unique_ptr<FreeStore> sFreeStore;
};

template <size_t Size, typename StorageType>
std::unique_ptr<FreeStore<Size,StorageType>> FreeStore<Size,StorageType>::sFreeStore = nullptr;
