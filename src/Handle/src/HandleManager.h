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


#ifndef pow2_datastructures_HandleManager_h
#define pow2_datastructures_HandleManager_h

#include "Handle.h"

namespace pow2
{

class HandleManager
{
public:
	enum { MaxEntries = 4096 }; // 2^12

	HandleManager();

	void Reset();	
	Handle Add(void* p, uint32 type);
	void Update(Handle handle, void* p);
	void Remove(Handle handle);
	
	void* Get(Handle handle) const;
	bool Get(Handle handle, void*& out) const;
	template< typename T > bool GetAs(Handle handle, T& out) const;

	int GetCount() const;

private:
	HandleManager(const HandleManager&);
	HandleManager& operator=(const HandleManager&);

	struct HandleEntry
	{
		HandleEntry();
		explicit HandleEntry(uint32 nextFreeIndex);
		
		uint32 m_nextFreeIndex : 12;
		uint32 m_counter : 15;
		uint32 m_active : 1;
		uint32 m_endOfList : 1;
		void* m_entry;
	};

	HandleEntry m_entries[MaxEntries];

	int m_activeEntryCount;
	uint32 m_firstFreeEntry;
};


template< typename T >
inline bool HandleManager::GetAs(Handle handle, T& out) const
{
	void* outAsVoid;
	const bool rv = Get(handle, outAsVoid);
	out = union_cast< T >(outAsVoid);

	return rv;
}


}

#endif
