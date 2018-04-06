//
//  HeapPolicy.h
//  PoolAllocator
//
//  Created by Mike Allison on 2/26/17.
//  Copyright (c) 2017 Mike Allison. All rights reserved.
//

#pragma once
#include "AllocatorTraits.hpp"
#include "IAllocator.h"

template<typename T>
class HeapAllocator
{
public:
	
	ALLOCATOR_TRAITS(T)
	
	template<typename U>
	struct rebind
	{
		typedef HeapAllocator<U> other;
	};
	
	// Default Constructor
	HeapAllocator(void) = default;
	
	// Copy Constructor
	template<typename U>
	HeapAllocator(HeapAllocator<U> const& other){}
	
	// Allocate memory
	pointer allocate(size_type count, const_pointer hint = 0)
	{
		if(count > max_size()){throw std::bad_alloc();}
		return static_cast<pointer>(::operator new(count * sizeof(type), ::std::nothrow));
	}
	
	// Delete memory
	void deallocate(pointer ptr, size_type count)
	{
		::operator delete(ptr);
	}
	
	// Max number of objects that can be allocated in one call
	size_type max_size(void) const {return max_allocations<sizeof(T)>::value;}
    
    size_t capacity(){ return max_size(); }
};
