#pragma once

#include <vector>
#include "Allocator.hpp"
#include "HeapPolicy.hpp"
#include "ObjectTraits.hpp"

#define POOL_INDEX_BITS 16

#define SLOT_SERIAL_BITS 16
#define SLOT_INDEX_BITS 31
#define SLOT_MOVED_BITS 1
#define SLOT_ALIVE_BITS 1

struct SparseSlotIndex {
	uint64_t : 16; //unused
	uint64_t alive : SLOT_ALIVE_BITS;
	uint64_t slot_serial : SLOT_SERIAL_BITS;
	uint64_t dense_slot_index : SLOT_INDEX_BITS;
};

struct DenseSlotIndex { 
	uint64_t alive : SLOT_ALIVE_BITS;
	uint64_t sparse_slot_index : SLOT_INDEX_BITS;
};

struct Handle {

	Handle() { reset(); }
	Handle( uint64_t serialized ) {
		pool_id = serialized >> (32 + 16);
		slot_serial = serialized >> 32;
		slot_index = serialized;
	}

	uint64_t pool_id : 16;
	uint64_t slot_serial : 16;
	uint64_t slot_index : 32;

	//explicit conversion
	inline operator uint64_t() const {
		return pool_id << (32+16) | slot_serial << 32 | slot_index;
	}

	inline bool isSet() {
		return ( pool_id != std::numeric_limits<uint16_t>::max() && slot_serial != std::numeric_limits<uint16_t>::max() && slot_index != std::numeric_limits<uint32_t>::max() );
	}

	inline void reset() { pool_id = -1; slot_serial = -1; slot_index = -1; }
};

class IDeferredReclaimationMemoryPolicy {
public:
	virtual bool free(Handle handle) = 0;
	virtual size_t size() const = 0;
	virtual void clear() = 0;
	virtual void reserve(size_t count) = 0;
	virtual void collect() = 0;
	virtual ~IDeferredReclaimationMemoryPolicy() = default;
};

template<typename T>
class SparseSet : public IDeferredReclaimationMemoryPolicy {

public:

	using iterator = typename std::vector<T>::iterator;
	using const_iterator = typename std::vector<T>::const_iterator;

	inline void collect() {
		//reclaim all memory

		if (mUncollected == 0)
			return;

		if (mUncollected == mBack) {
			//everything is completely reclaimed
			mUncollected = 0;
			mBack = 0;
			return;
		}

		for (int i = 0; i < mBack; i++) {

			auto & d = mDense[i];

			if (!d.alive) {

				auto last_dense = &mDense[mBack - 1];
				while (!last_dense->alive) {
					//need a live one to swap with.
					--mBack;
					last_dense = &mDense[mBack - 1];
				}

				auto& cur_sparse = mSparse[d.sparse_slot_index];

				auto& cur_data = mData[cur_sparse.dense_slot_index];
				auto& last_alive_data = mData[mBack - 1];

				//swap data
				std::swap(std::move(cur_data), std::move(last_alive_data));

				auto& other_sparse = mSparse[last_dense->sparse_slot_index];

				//swap dense slot
				std::swap(std::move(d), std::move(*last_dense));

				//just swap sparse indices, not slot serial and alive is already set
				auto tmp = cur_sparse.dense_slot_index;
				cur_sparse.dense_slot_index = other_sparse.dense_slot_index;
				other_sparse.dense_slot_index = tmp;

				--mBack;
			}
		}
		mUncollected = 0;
	}

	template<typename...Args>
	inline Handle alloc(Args&&...args) {

		if (mBack >= mData.size()) {
			//grow as needed...slow if happens but dynamic
			reserve(mData.size() + 1024);
		}

		auto & next_data = mData[mBack];
		auto & next_dense = mDense[mBack];
		auto & available_sparse = mSparse[next_dense.sparse_slot_index];

		next_dense.alive = 1;
		available_sparse.alive = 1;

		Handle hndl;
		//TODO pool ids
		hndl.pool_id = 0;
		hndl.slot_index = next_dense.sparse_slot_index;
		hndl.slot_serial = available_sparse.slot_serial;
		mBack++;

		new(&next_data) T(args...);

		return std::move(hndl);
	}

	inline bool free( Handle handle ) override {
		if (isValid(handle) && mBack >= 1) {
			
			//mark as dead and invalidate all handles
			auto & s = mSparse[handle.slot_index];
			s.alive = 0;
			s.slot_serial++;
			mDense[s.dense_slot_index].alive = 0;
			mUncollected++;

			mData[s.dense_slot_index].~T();

			return true;
		}
		else {
			return false;
		}
	}

	inline bool isValid(Handle handle) {
		if (!handle.isSet())return false;
		return ( mSparse[handle.slot_index].alive && handle.slot_serial == mSparse[handle.slot_index].slot_serial );
	}

	inline T* get(Handle handle) {
		if (isValid(handle)) {
			return &mData[mSparse[handle.slot_index].dense_slot_index];
		}
		else {
			return nullptr;
		}
	}

	inline size_t size() const override { return mBack-mUncollected; }
	inline size_t capacity() const { return mData.size(); }
	inline bool needs_collection() const { return mUncollected > 0; }

	inline iterator begin() { return mData.begin(); }
	inline iterator end() { auto end = mData.begin(); std::advance(end, mBack); return end; }

	inline const_iterator cbegin() { return mData.cbegin(); }
	inline const_iterator cend() { auto cend = mData.cbegin(); std::advance(cend, mBack); return cend; }

	inline void reserve(size_t count)override {
		mData.resize(count);
		mSparse.resize(count);
		mDense.resize(count);
		for (int i = mBack; i < count; i++) {
			mSparse[i].dense_slot_index = i;
		}
		for (int i = mBack; i < count; i++) {
			mDense[i].sparse_slot_index = i;
		}
	}

	inline void clear() override {
		mData.clear();
		mSparse.clear();
		mDense.clear();
		mBack = 0;
	}

private:

	size_t mBack{0};
	size_t mUncollected{0};
	std::vector<SparseSlotIndex> mSparse;
	std::vector<DenseSlotIndex> mDense;
	std::vector<T, Allocator<T, heap_policy<T>, basic_object_traits<T>>> mData;

};
