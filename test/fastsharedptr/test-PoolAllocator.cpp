//
//  test-PoolAllocator.cpp
//  MemoryManagement
//
//  Created by Michael Allison on 4/5/18.
//

#include <stdio.h>
#include <iostream>
#include <random>
#include <algorithm>
#include "catch.hpp"
#include <fastsharedptr/PoolAllocator.hpp>
#include <fastsharedptr/Allocator.hpp>
#include <fastsharedptr/BasicObjectTraits.hpp>

class Object {
public:
    Object():mCount(5){}
    int getCount(){return mCount;}
    void inc(){mCount++;}
private:
    int mCount{5};
};

unsigned int randInt(unsigned int max) {
    double mix = (double)rand() / (double)RAND_MAX;
    return (unsigned int)floor((mix * (double)max));
}

TEST_CASE("PoolAllocatorFixed","[allocator]"){

    using FixedSizeObjectStorage = fixed_size<Object, 1000>;
    using PoolAllocator = pool_allocator<Object, FixedSizeObjectStorage>;
    using Alloc = Allocator<Object, PoolAllocator, basic_object_traits<Object>>;
    
    Alloc alloc;
    
    auto obj1 = alloc.allocate();
    alloc.construct(obj1);
    auto obj2 = alloc.allocate();
    alloc.construct(obj2);
    auto obj3 = alloc.allocate();
    alloc.construct(obj3);
    auto obj4 = alloc.allocate();
    alloc.construct(obj4);
    REQUIRE(obj1->getCount() == 5);
    REQUIRE(obj2->getCount() == 5);
    REQUIRE(obj3->getCount() == 5);
    REQUIRE(obj4->getCount() == 5);
    
    obj1->inc();
    REQUIRE(obj1->getCount() == 6);

    obj3->inc();
    obj3->inc();
    obj2->inc();
    obj2->inc();
    REQUIRE(obj3->getCount() == 7);
    REQUIRE(obj2->getCount() == 7);

    alloc.destroy(obj3);
    alloc.deallocate(obj3);
    alloc.destroy(obj2);
    alloc.deallocate(obj2);

    auto obj5 = alloc.allocate();
    alloc.construct(obj5);
    auto obj6 = alloc.allocate();
    alloc.construct(obj6);

    REQUIRE(obj5 == obj2);
    REQUIRE(obj6 == obj3);
    
    REQUIRE(obj5->getCount() == 5);
    REQUIRE(obj6->getCount() == 5);
    
    alloc.destroy(obj1);
    alloc.destroy(obj4);
    alloc.destroy(obj5);
    alloc.destroy(obj6);
    alloc.deallocate(obj1);
    alloc.deallocate(obj4);
    alloc.deallocate(obj5);
    alloc.deallocate(obj6);
    
    std::vector<Object*> mObjects;
    auto num = alloc.capacity();
    for(int i = 0; i < num; i++){
        auto ptr = alloc.allocate();
        alloc.construct(ptr);
        mObjects.push_back(ptr);
        mObjects.back()->inc();
        REQUIRE(mObjects.back()->getCount() == 6);
    }
    
    bool caught = false;
    try{
        auto ptr = alloc.allocate();
    }catch(const std::bad_alloc& ex){
        caught = true;
    }

    REQUIRE(caught == true);

    std::vector<int> set;
    for(int i = 0; i < mObjects.size(); i++){
        set.push_back(i);
    }
    
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(set.begin(), set.end(), g);
    
    for(int i = 0; i < set.size(); i++){
        auto it = mObjects.begin();
        std::advance(it, set[i]);
        alloc.destroy(*it);
        alloc.deallocate(*it);
    }
    
    for (unsigned i = set.size(); i-- > 0; ){
        auto it = mObjects.begin();
        std::advance(it, set[i]);
        auto ptr = alloc.allocate();
        alloc.construct(ptr);
        REQUIRE(ptr == *it);
    }
}


TEST_CASE("PoolAllocatorDynamic","[allocator]"){
    
    using DynamicObjectStorage = dynamic_size<Object, 500>;
    using PoolAllocator = pool_allocator<Object, DynamicObjectStorage>;
    using Alloc = Allocator<Object, PoolAllocator, basic_object_traits<Object>>;
    
    Alloc alloc;
    
    auto obj1 = alloc.allocate();
    alloc.construct(obj1);
    auto obj2 = alloc.allocate();
    alloc.construct(obj2);
    auto obj3 = alloc.allocate();
    alloc.construct(obj3);
    auto obj4 = alloc.allocate();
    alloc.construct(obj4);
    REQUIRE(obj1->getCount() == 5);
    REQUIRE(obj2->getCount() == 5);
    REQUIRE(obj3->getCount() == 5);
    REQUIRE(obj4->getCount() == 5);
    
    obj1->inc();
    REQUIRE(obj1->getCount() == 6);
    
    obj3->inc();
    obj3->inc();
    obj2->inc();
    obj2->inc();
    REQUIRE(obj3->getCount() == 7);
    REQUIRE(obj2->getCount() == 7);
    
    alloc.destroy(obj3);
    alloc.deallocate(obj3);
    alloc.destroy(obj2);
    alloc.deallocate(obj2);
    
    auto obj5 = alloc.allocate();
    alloc.construct(obj5);
    auto obj6 = alloc.allocate();
    alloc.construct(obj6);
    
    REQUIRE(obj5 == obj2);
    REQUIRE(obj6 == obj3);
    
    REQUIRE(obj5->getCount() == 5);
    REQUIRE(obj6->getCount() == 5);
    
    alloc.destroy(obj1);
    alloc.destroy(obj4);
    alloc.destroy(obj5);
    alloc.destroy(obj6);
    alloc.deallocate(obj1);
    alloc.deallocate(obj4);
    alloc.deallocate(obj5);
    alloc.deallocate(obj6);
    
    std::vector<Object*> mObjects;
    auto num = alloc.capacity();
    for(int i = 0; i < num; i++){
        auto ptr = alloc.allocate();
        alloc.construct(ptr);
        mObjects.push_back(ptr);
        mObjects.back()->inc();
        REQUIRE(mObjects.back()->getCount() == 6);
    }

    std::vector<int> set;
    for(int i = 0; i < mObjects.size(); i++){
        set.push_back(i);
    }
    
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(set.begin(), set.end(), g);
    
    for(int i = 0; i < set.size(); i++){
        auto it = mObjects.begin();
        std::advance(it, set[i]);
        alloc.destroy(*it);
        alloc.deallocate(*it);
    }
    
    for (unsigned i = set.size(); i-- > 0; ){
        auto it = mObjects.begin();
        std::advance(it, set[i]);
        auto ptr = alloc.allocate();
        alloc.construct(ptr);
        REQUIRE(ptr == *it);
    }
    
    auto newblock = alloc.allocate();
    alloc.construct(newblock);
    
    REQUIRE(alloc.capacity() == num * 2);
    
    for(int i = 0; i < num; i++){
        alloc.allocate();
    }
    
    REQUIRE(alloc.capacity() == num * 3);

}

TEST_CASE("FastSharedPtr","[allocator]"){

    using FixedSizeObjectStorage = fixed_size<Object, 5000>;
    using PoolAllocator = pool_allocator<Object, FixedSizeObjectStorage>;
    using Alloc = Allocator<Object, PoolAllocator, basic_object_traits<Object>>;
    
    Alloc myAlloc;

    auto shared1 = std::shared_ptr<Object>( new Object, std::default_delete<Object>(), myAlloc );
    auto shared2 = std::shared_ptr<Object>( new Object, std::default_delete<Object>(), myAlloc );
    auto shared3 = std::shared_ptr<Object>( new Object, std::default_delete<Object>(), myAlloc );
    auto shared4 = std::shared_ptr<Object>( new Object, std::default_delete<Object>(), myAlloc );
    auto shared5 = std::shared_ptr<Object>( new Object, std::default_delete<Object>(), myAlloc );

    
    
}
