//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include <stdlib.h>
#include <string.h>

//--------------------------------------
#ifdef new
#undef new
#endif

void* FN_CDECL operator new(dsize_t dt, void* ptr)
{
   return (ptr);
}   


//--------------------------------------
void* dRealMalloc(dsize_t in_size)
{
   return malloc(in_size);
}


//--------------------------------------
void dRealFree(void* in_pFree)
{
   free(in_pFree);
}


void* dMemcpy(void *dst, const void *src, dsize_t size)
{
   return memcpy(dst,src,size);
}   


//--------------------------------------
void* dMemmove(void *dst, const void *src, dsize_t size)
{
   return memmove(dst,src,size);
}  
 
//--------------------------------------
void* dMemset(void *dst, int c, dsize_t size)
{
   return memset(dst,c,size);   
}   

//--------------------------------------
int dMemcmp(const void *ptr1, const void *ptr2, dsize_t len)
{
   return(memcmp(ptr1, ptr2, len));
}
