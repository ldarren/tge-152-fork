//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------
// Copied from NVidia Triangle Strip SDK, available from NVidia website
//-----------------------------------------------------------------------------
  
#ifndef VERTEX_CACHE_H
#define VERTEX_CACHE_H

#include "../DTSPlusTypes.h"

class VertexCache
{
	
public:
	
	VertexCache(S32 size)
	{
		numEntries = size;
		
		entries = new S32[numEntries];
		
		for(S32 i = 0; i < numEntries; i++)
			entries[i] = -1;
	}
		
	VertexCache() { VertexCache(16); }
	~VertexCache() { delete[] entries; entries = 0; }
	
	bool InCache(S32 entry)
	{
		bool returnVal = false;
		for(S32 i = 0; i < numEntries; i++)
		{
			if(entries[i] == entry)
			{
				returnVal = true;
				break;
			}
		}
		
		return returnVal;
	}
	
	S32 AddEntry(S32 entry)
	{
		S32 removed;
		
		removed = entries[numEntries - 1];
		
		//push everything right one
		for(S32 i = numEntries - 2; i >= 0; i--)
		{
			entries[i + 1] = entries[i];
		}
		
		entries[0] = entry;
		
		return removed;
	}

	void Clear()
	{
		memset(entries, -1, sizeof(S32) * numEntries);
	}
	
	void Copy(VertexCache* inVcache) 
	{
		for(S32 i = 0; i < numEntries; i++)
		{
			inVcache->Set(i, entries[i]);
		}
	}

	S32 At(S32 index) { return entries[index]; }
	void Set(S32 index, S32 value) { entries[index] = value; }

private:

  S32 *entries;
  S32 numEntries;

};

#endif
