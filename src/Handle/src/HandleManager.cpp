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
#include "Pow2Assert.h"
#include <cstddef>


namespace pow2
{



HandleManager::HandleEntry::HandleEntry() 
	: m_nextFreeIndex(0)
	, m_counter(1)
	, m_active(0)
	, m_endOfList(0)
	, m_entry(NULL)
{}

HandleManager::HandleEntry::HandleEntry(uint32 nextFreeIndex)
	: m_nextFreeIndex(nextFreeIndex)
	, m_counter(1)
	, m_active(0)
	, m_endOfList(0)
	, m_entry(NULL)
{}



HandleManager::HandleManager()
{
	Reset();
}


void HandleManager::Reset()
{
	m_activeEntryCount = 0;
	m_firstFreeEntry = 0;

	for (int i = 0; i < MaxEntries - 1; ++i)
		m_entries[i] = HandleEntry(i + 1);
	m_entries[MaxEntries - 1] = HandleEntry();
	m_entries[MaxEntries - 1].m_endOfList = true;
}


Handle HandleManager::Add(void* p, uint32 type)
{
	POW2_ASSERT(m_activeEntryCount < MaxEntries - 1);
	POW2_ASSERT(type >= 0 && type <= 31);

	const int newIndex = m_firstFreeEntry;
	POW2_ASSERT(newIndex < MaxEntries);
	POW2_ASSERT(m_entries[newIndex].m_active == false);
	POW2_ASSERT(!m_entries[newIndex].m_endOfList);

	m_firstFreeEntry = m_entries[newIndex].m_nextFreeIndex;
	m_entries[newIndex].m_nextFreeIndex = 0;
	m_entries[newIndex].m_counter = m_entries[newIndex].m_counter + 1;
	if (m_entries[newIndex].m_counter == 0)
		m_entries[newIndex].m_counter = 1;
	m_entries[newIndex].m_active = true;
	m_entries[newIndex].m_entry = p;

	++m_activeEntryCount;

	return Handle (newIndex, m_entries[newIndex].m_counter, type);
}


void HandleManager::Update(Handle handle, void* p)
{
	const int index = handle.m_index;
	POW2_ASSERT(m_entries[index].m_counter == handle.m_counter);
	POW2_ASSERT(m_entries[index].m_active == true);

	m_entries[index].m_entry = p;
}


void HandleManager::Remove(const Handle handle)
{
	const uint32 index = handle.m_index;
	POW2_ASSERT(m_entries[index].m_counter == handle.m_counter);
	POW2_ASSERT(m_entries[index].m_active == true);

	m_entries[index].m_nextFreeIndex = m_firstFreeEntry;
	m_entries[index].m_active = 0;
	m_firstFreeEntry = index;

	--m_activeEntryCount;
}


void* HandleManager::Get(Handle handle) const
{
	void* p = NULL;
	if (!Get(handle, p))
		return NULL;
	return p;
}


bool HandleManager::Get(const Handle handle, void*& out) const
{
	const int index = handle.m_index;
	if (m_entries[index].m_counter != handle.m_counter ||
	    m_entries[index].m_active == false)
		return false;

	out = m_entries[index].m_entry;
	return true;
}


int HandleManager::GetCount() const
{
	return m_activeEntryCount;
}


}
