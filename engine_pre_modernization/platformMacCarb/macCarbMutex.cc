//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include "platform/platform.h"
#include "platform/platformMutex.h"
#include "platform/platformThread.h"

// TODO: examine & dump errno if pthread_* funcs fail. ( only in debug build )

typedef struct MacCarbMutex
{
   pthread_mutex_t   mMutex;
   bool              locked;
   U32         lockedByThread;
};

void* Mutex::createMutex(void)
{
   bool ok;
   MacCarbMutex *m = new MacCarbMutex();
   pthread_mutexattr_t attr;
   ok = pthread_mutexattr_init(&attr);
   ok = pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
   ok = pthread_mutex_init(&(m->mMutex),&attr);
   AssertFatal(ok == 0, "Mutex::createMutex() failed: pthread_mutex_init() failed.");
   
   m->locked = false;
   m->lockedByThread = 0;
   
   return (void*)m;
}

void Mutex::destroyMutex(void* mutex)
{
   bool ok;
   MacCarbMutex *m = (MacCarbMutex*)mutex;
   ok = pthread_mutex_destroy( &(m->mMutex) );
   AssertFatal(ok == 0, "Mutex::destroyMutex() failed: pthread_mutex_destroy() failed.");
   delete mutex;
}
 
bool Mutex::lockMutex( void *mutex, bool block)
{
   int ok;
   MacCarbMutex *m = (MacCarbMutex*)mutex;

   if(block)
   {
      ok = pthread_mutex_lock( &(m->mMutex) );
      AssertFatal( ok != EINVAL, "Mutex::lockMutex() failed: invalid mutex.");
      AssertFatal( ok != EDEADLK, "Mutex::lockMutex() failed: system detected a deadlock!");
      AssertFatal( ok == 0, "Mutex::lockMutex() failed: pthread_mutex_lock() failed -- unknown reason.");
   } 
   else {
      ok = pthread_mutex_trylock( &(m->mMutex) );
      // returns EBUSY if mutex was locked by another thread,
      // returns EINVAL if mutex was not a valid mutex pointer,
      // returns 0 if lock succeeded.
      AssertFatal( ok != EINVAL, "Mutex::lockMutex(non blocking) failed: invalid mutex.");
      if( ok != 0 )
         return false;

      AssertFatal( ok == 0, "Mutex::lockMutex(non blocking) failed: pthread_mutex_trylock() failed -- unknown reason.");
   }

   m->locked = true;
   m->lockedByThread = Thread::getCurrentThreadId();
   return true;
}

void Mutex::unlockMutex( void *mutex)
{
   bool ok;
   MacCarbMutex *m = (MacCarbMutex*)mutex;
   ok = pthread_mutex_unlock( &(m->mMutex) );
   AssertFatal( ok == 0, "Mutex::unlockMutex() failed: pthread_mutex_unlock() failed.");
   m->locked = false;
   m->lockedByThread = 0;
}
