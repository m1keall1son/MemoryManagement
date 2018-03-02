/*
* Copyright (c) 2008, Power of Two Games LLC
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Power of Two Games LLC nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY POWER OF TWO GAMES LLC ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL POWER OF TWO GAMES LLC BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "HandleManager.h"
#include <UnitTest++.h>

namespace pow2 { namespace
{

TEST(InvalidHandleReturnsFalse)
{
	HandleManager mgr;
	void* out;
	CHECK_EQUAL(false, mgr.Get(Handle(123,0,0), out));
}


TEST(CorrectPointerReturned)
{
	void* p = (void*)0x124384;
	HandleManager mgr;
	const Handle h = mgr.Add(p, 0);
	CHECK_EQUAL(p, mgr.Get(h));
}


TEST(CorrectPointerReturnedForPointers)
{
	void* p = (void*)0x124384;

	HandleManager mgr;
	const Handle h = mgr.Add(p, 0);

	void* out;
	const bool ok = mgr.Get(h, out);

	CHECK(ok);
	CHECK_EQUAL(p, out);
}


TEST(CorrectPointerReturnedWithMultipleEntries)
{
	void* p0 = (void*)0x124384;
	void* p1 = (void*)0x437434;

	HandleManager mgr;
	const Handle h0 = mgr.Add(p0, 0);
	const Handle h1 = mgr.Add(p1, 0);

	void* out0;
	CHECK(mgr.Get(h0, out0));
	CHECK_EQUAL(p0, out0);
	
	void* out1;
	CHECK(mgr.Get(h1, out1));
	CHECK_EQUAL(p1, out1);
}


TEST(NullIfEntryRemoved)
{
	void* p0 = (void*)0x124384;
	void* p1 = (void*)0x437434;

	HandleManager mgr;
	const Handle h0 = mgr.Add(p0, 0);
	const Handle h1 = mgr.Add(p1, 0);
	mgr.Remove(h0);	

	void* out;
	CHECK(mgr.Get(h0, out) == false);
	CHECK(mgr.Get(h1, out));
	CHECK_EQUAL(p1, out);
}


TEST(AssertIfRemovingNonExistingEntry)
{
	HandleManager mgr;
	mgr.Add((void*)0x421434, 0);
	CHECK_ASSERT(mgr.Remove(Handle(747,0,0)));
}


TEST(AssertIfTooManyItemsAdded)
{
	HandleManager mgr;
	for (int i=0; i<HandleManager::MaxEntries-1; ++i)
		mgr.Add((void*)0x421434, 0);
	CHECK_ASSERT(mgr.Add((void*)0x421434, 0));
}


TEST(ValidHandleIfItemRemovedAndMoreItemsAdded)
{
	HandleManager mgr;
	const Handle h = mgr.Add((void*)0x421434, 0);
	for (int i=0; i<HandleManager::MaxEntries-2; ++i)
		mgr.Add((void*)0x421434, 0);
	mgr.Remove(h);
	const Handle newHandle = mgr.Add((void*)0x421434, 0);
	CHECK(newHandle != 0);
}


TEST(HandleReturnedForSameSpotIsDifferent)
{
	HandleManager mgr;
	const Handle h = mgr.Add((void*)0x421434, 0);
	for (int i=0; i<HandleManager::MaxEntries-2; ++i)
		mgr.Add((void*)0x421434, 0);
	mgr.Remove(h);
	const Handle newHandle = mgr.Add((void*)0x421434, 0);
	CHECK_EQUAL(h.m_index, newHandle.m_index);
	CHECK(h.m_counter != newHandle.m_counter);
}


TEST(GettingDeletedHandleWithSameSlotReturnsFalse)
{
	HandleManager mgr;
	const Handle h = mgr.Add((void*)0x421434, 0);
	for (int i=0; i<HandleManager::MaxEntries-2; ++i)
		mgr.Add((void*)0x421434, 0);

	mgr.Remove(h);
	mgr.Add((void*)0x421434, 0);

	void* out;
	CHECK(mgr.Get(h, out) == false);
}


TEST(DeletingDeletedHandleWithSameSlotAsserts)
{
	HandleManager mgr;
	const Handle h = mgr.Add((void*)0x421434, 0);
	for (int i=0; i<HandleManager::MaxEntries-2; ++i)
		mgr.Add((void*)0x421434, 0);
	mgr.Remove(h);
	mgr.Add((void*)0x421434, 0);
	CHECK_ASSERT(mgr.Remove(h));
}


TEST(NoEntriesByDefault)
{
	HandleManager mgr;
	CHECK_EQUAL(0, mgr.GetCount());
}


TEST(CountGoesUpWhenAdding)
{
	HandleManager mgr;
	mgr.Add((void*)1, 0);
	mgr.Add((void*)2, 0);
	CHECK_EQUAL(2, mgr.GetCount());
}


TEST(GoesDownWhenRemoving)
{
	HandleManager mgr;
	mgr.Add((void*)1, 0);
	const Handle h = mgr.Add((void*)1, 0);
	mgr.Remove(h);
	CHECK_EQUAL(1, mgr.GetCount());
}


TEST(UpdateOfInvalidHandleAsserts)
{
	HandleManager mgr;
	const Handle h = mgr.Add((void*)3, 0);
	CHECK_ASSERT(mgr.Update(Handle(h.m_index+1,0,0), (void*)4));
}


TEST(UpdateChangesValueOfExistingHandle)
{
	HandleManager mgr;
	const Handle h = mgr.Add((void*)3, 0);
	mgr.Update(h, (void*)4);

	void* out;
	CHECK(mgr.Get(h, out));
	CHECK_EQUAL(4, (int)out);	
}


TEST(HandleWithCorrectBitfieldReturned)
{
	void* p = (void*)0x124384;
	HandleManager mgr;
	const uint32 type = 0x5;
	const Handle h = mgr.Add(p, type);
	CHECK_EQUAL(type, h.m_type);
}


TEST(ResetRemovesAllHandles)
{
	HandleManager mgr;
	const Handle h = mgr.Add((void*)1, 0);
	mgr.Reset();
	CHECK_EQUAL(0, mgr.GetCount());
	CHECK_EQUAL((void*)NULL, mgr.Get(h));
}


}}
