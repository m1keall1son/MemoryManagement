#include "catch.hpp"

#include "TestClass.h"
#include "SparseSet.hpp"
#include <iostream>

TEST_CASE( "Sparse Set", "[memory]" ) {

	SparseSet<Test> set;
	set.reserve(500);

	SECTION("Alloc/Free") {

		REQUIRE(set.size() == 0);

		auto handle = set.alloc();

		REQUIRE(set.size() == 1);
		REQUIRE(set.free(handle));
		REQUIRE_FALSE(set.free(handle));

		REQUIRE(set.size() == 0);
	}

	SECTION("Handle validity") {

		Handle h;

		auto s = h.isSet();
		REQUIRE_FALSE(h.isSet());
		
		auto h2 = set.alloc();

		REQUIRE(h2.isSet());
		REQUIRE(set.isValid(h2));

		h = h2;

		REQUIRE(h.isSet());
		REQUIRE( h == h2 );

		h.reset();

		REQUIRE(h == std::numeric_limits<uint64_t>::max());

		auto h3 = h2;

		set.free(h3);

		REQUIRE(set.size() == 0);

		REQUIRE_FALSE(set.isValid(h2));

		//handle conversions
		int iterations = 100;
		std::vector<Handle> handles(iterations);
		for (int i = 0; i < iterations; i++ ) {
		
			auto it = set.alloc();

			uint16_t pool_id = it.pool_id;
			uint16_t slot_serial = it.slot_serial;
			uint32_t slot_index = it.slot_index;

			uint64_t converted = it;

			auto data_test = Handle(converted);

			REQUIRE(data_test.pool_id == pool_id);
			REQUIRE(data_test.slot_serial == slot_serial);
			REQUIRE(data_test.slot_index == slot_index);
		
			handles[i] = it;

		}

		for ( auto &it : handles ) {
			set.free(it);
		}

	}

	SECTION("Handle dereferenceing") {

		auto handle = set.alloc(200);

		auto obj = set.get(handle);

		REQUIRE(obj);

		REQUIRE(obj->getVal() == 200);

		obj->setVal(2);

		auto again = set.get(handle);

		REQUIRE(again);

		REQUIRE(obj->getVal() == again->getVal());

		set.free(handle);

		auto failed_get = set.get(handle);

		REQUIRE_FALSE(failed_get);

	}

	SECTION("Set iterating") {

		REQUIRE(set.size() == 0);

		int iterations = 100;
		std::vector<Handle> handles(iterations);

		for (int i = 0; i < iterations; i++) {
			handles[i] = set.alloc(rand());
		}

		REQUIRE(set.size() == iterations);

		auto it = handles.begin();
		int count = 0;
		for (auto & t : set) {

			auto val = *set.get( *it++ );
			REQUIRE(val.getVal() == t.getVal());
			count++;
		}

		REQUIRE(count == iterations);

		for (auto &h : handles) {
			REQUIRE(set.free(h));
		}

		REQUIRE(set.size() == 0);

	}

	SECTION("Set collection") {

		REQUIRE(set.size() == 0);

		int iterations = 100;
		std::vector<Handle> handles(iterations);

		for (int i = 0; i < iterations; i++) {
			handles[i] = set.alloc(rand());
		}

		REQUIRE(set.size() == iterations);

		auto rand = randInt(handles.size());
		auto max_size = set.size();
		auto destroyed = 0;
		for (int i = 0; i < rand; i++ ) {
			if (set.free(handles[randInt(max_size)])) {
				destroyed++;
			}
		}

		REQUIRE(set.size() == max_size - destroyed);

		set.collect();

		REQUIRE(set.size() == max_size - destroyed);
		auto current_size = set.size();

		auto refill_some = randInt(set.capacity() - set.size());
		for (int i = 0; i < refill_some; i++) {
			handles.push_back( set.alloc(i+set.size()) );
		}

		REQUIRE(set.size() == current_size + refill_some);
		current_size = set.size();
		auto destroy_some = randInt(refill_some);
		destroyed = 0;
		for (int i = 0; i < destroy_some; i++) {
			if (set.free(handles[randInt(max_size)])) {
				destroyed++;
			}
		}

		REQUIRE(set.size() == current_size - destroyed);
		current_size = set.size();

		refill_some = randInt(set.capacity() - set.size());
		for (int i = 0; i < refill_some; i++) {
			handles.push_back(set.alloc(i + set.size()));
		}

		REQUIRE(set.size() == current_size + refill_some);

		set.collect();

		int freed = 0;
		current_size = set.size();
		for ( auto &h : handles ) {
			if (set.free(h))
				freed++;
		}

		REQUIRE(freed == current_size);
		REQUIRE(set.size() == 0);

		set.collect();

		REQUIRE(freed == current_size);
		REQUIRE(set.size() == 0);

	}


}