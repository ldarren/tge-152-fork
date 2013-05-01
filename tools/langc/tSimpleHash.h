//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// [tom, 4/26/2005] This was based on SparseArray ...
//
// SparseArray deletes the objects contained within it, and so doesnt really
// work for our purposes. Although SparseArray is not used anywhere in the
// engine, I figured it would be best to copy it and hack the copy rather
// then risk anyone's code breaking for no real reason.

#ifndef _TSIMPLEHASH_H_
#define _TSIMPLEHASH_H_

//Includes
#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _PLATFORMASSERT_H_
#include "platform/platformAssert.h"
#endif

template <class T>
class SimpleHash
{
  protected:
   struct Node {
      T*    pObject;
      U32   key;

      Node* next;
   };

  protected:
   U32   mModulus;
   Node* mSentryTables;

   void clearTables();           // Note: _deletes_ the objects!

  public:
   SimpleHash(const U32 modulusSize = 64);
   ~SimpleHash();

   void insert(T* pObject, U32 key);
   T*   remove(U32 key);
   T*   retreive(U32 key);
};

template <class T>
inline SimpleHash<T>::SimpleHash(const U32 modulusSize)
{
   AssertFatal(modulusSize > 0, "Error, modulus must be > 0");

   mModulus      = modulusSize;
   mSentryTables = new Node[mModulus];
   for (U32 i = 0; i < mModulus; i++)
      mSentryTables[i].next = NULL;
}

template <class T>
inline SimpleHash<T>::~SimpleHash()
{
   clearTables();
}

template <class T>
inline void SimpleHash<T>::clearTables()
{
   for (U32 i = 0; i < mModulus; i++) {
      Node* pProbe = mSentryTables[i].next;
      while (pProbe != NULL) {
         Node* pNext = pProbe->next;
         //delete pProbe->pObject;
         delete pProbe;
         pProbe = pNext;
      }
   }
   delete [] mSentryTables;
   mSentryTables = NULL;
   mModulus = 0;
}

template <class T>
inline void SimpleHash<T>::insert(T* pObject, U32 key)
{
   U32 insert = key % mModulus;
   Node* pNew = new Node;
   pNew->pObject = pObject;
   pNew->key     = key;
   pNew->next    = mSentryTables[insert].next;
   mSentryTables[insert].next = pNew;

#ifdef TORQUE_DEBUG
   Node* probe = pNew->next;
   while (probe != NULL) {
      AssertFatal(probe->key != key, "error, duplicate keys in sparse array!");
      probe = probe->next;
   }
#endif
}

template <class T>
inline T* SimpleHash<T>::remove(U32 key)
{
   U32 remove  = key % mModulus;
   Node* probe = mSentryTables[remove];
   while (probe->next != NULL) {
      if (probe->next->key == key) {
         Node* remove = probe->next;
         T* pReturn   = remove->pObject;
         probe->next  = remove->next;
         delete remove;
         return pReturn;
      }
      probe = probe->next;
   }

   AssertFatal(false, "Key didn't exist in the array!");
   return NULL;
}

template <class T>
inline T* SimpleHash<T>::retreive(U32 key)
{
   U32 retrieve = key % mModulus;
   Node* probe  = &mSentryTables[retrieve];
   while (probe->next != NULL) {
      if (probe->next->key == key) {
         return probe->next->pObject;
      }
      probe = probe->next;
   }

   AssertFatal(false, "Key didn't exist in the array!");
   return NULL;
}

#endif //_TSIMPLEHASH_H_

