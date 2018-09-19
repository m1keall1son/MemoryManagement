//
//  test-PoolAllocator.cpp
//  MemoryManagement
//
//  Created by Michael Allison on 4/5/18.
//

#include <stdio.h>
#include <iostream>
#include <random>
#include <map>
#include <algorithm>
#include "catch.hpp"
#include "Allocator.hpp"
#include "FreeStoreAllocator.hpp"

class Object {
public:
    Object(int num = 0):mCount(5),mSomeString(std::to_string(num)){}
    int getCount(){return mCount;}
    void inc(){mCount++; mAnother++;}
    void print(){
        std::cout << mSomeString << std::endl;
    }
private:
    int mCount{5};
    float mAnother{0};
    std::string mSomeString;
};

unsigned int randInt(unsigned int max) {
    double mix = (double)rand() / (double)RAND_MAX;
    return (unsigned int)floor((mix * (double)max));
}

TEST_CASE("FreeStoreAllocatorFixed","[allocator]"){

    using Alloc = Allocator<Object, FreeStoreAllocator<Object, FixedSizeStorage,1000>>;

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


TEST_CASE("FreeStoreAllocatorDynamic","[allocator]"){

    using Alloc = Allocator<Object, FreeStoreAllocator<Object,BlockListStorage,1024>>;

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

TEST_CASE("FreeStoreSharedPtr","[allocator]"){

    using Alloc = Allocator<Object, FreeStoreAllocator<Object,FixedSizeStorage,1024>>;

    auto shared1 = std::allocate_shared<Object>( Alloc() );
    auto shared2 = std::allocate_shared<Object>( Alloc() );
    auto shared3 = std::allocate_shared<Object>( Alloc() );
    auto shared4 = std::allocate_shared<Object>( Alloc() );
    auto shared5 = std::shared_ptr<Object>( new Object, std::default_delete<Object>(), Alloc() );

    REQUIRE(shared1.get() != shared2.get());

    {
        auto cpy = shared2;
        REQUIRE(shared2.get() == cpy.get());
        REQUIRE(shared2.use_count() == 2);
    }

    REQUIRE(shared2.use_count() == 1);
    shared5.reset();
    REQUIRE(shared5.get() == nullptr);

    auto weak = std::weak_ptr<Object>(shared1);
    if(auto sp = weak.lock()){
        sp->inc();
    }

    REQUIRE(shared1->getCount() == 6);
    shared1.reset();
    REQUIRE(weak.expired());

}

//TODO, instead of passing the max number of bytes, pass how many objects, that way we can account for allocator rebinds
//since in many cases we wont know the sizes
//or maybe there are two flavors...

TEST_CASE("FreeStore std::list","[allocator]"){
    using Alloc = Allocator<Object, FreeStoreAllocator<Object,FixedSizeStorage,(sizeof(Object) + sizeof(Object*)*2)*10>>;
    auto list = std::list<Object,Alloc>(Alloc());
    auto max_bytes = FixedSizeStorage<(sizeof(Object) + sizeof(Object*)*2),(sizeof(Object) + sizeof(Object*)*2)*10>::BLOCK_SIZE;
    auto fits = FixedSizeStorage<(sizeof(Object) + sizeof(Object*)*2),(sizeof(Object) + sizeof(Object*)*2)*10>::OBJECTS_PER_BLOCK;
    for( int i = 0; i < fits; i++ ){
        list.emplace_back(i);
        list.back().print();
    }
    REQUIRE_THROWS(list.emplace_back());
}

TEST_CASE("FreeStore std::map","[allocator]"){
    using Alloc = Allocator<std::pair<const size_t, Object>, FreeStoreAllocator<std::pair<const size_t, Object>,FixedSizeStorage,1024>>;
    auto map = std::map<size_t,Object,std::less<size_t>,Alloc>(Alloc());
    for( int i = 0; i < 10; i++ ){
        auto it = map.insert({i,Object(i)});
        it.first->second.print();
    }
}

