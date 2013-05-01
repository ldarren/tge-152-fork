//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// platformSemaphore.h does not ask for any inter process communication,
// and the posix semaphores require a file to be created on disk.
// which could create annoyances if the appication crashes...
// so we'll just roll our own semaphore here.

// note: this is not a bulletproof solution to the starving problem...
// see: The Little Book of Semapores, by Allen B. Downey, at http://greenteapress.com/semaphores/

#include <pthread.h>
#include "platform/platform.h"
#include "platform/platformSemaphore.h"



typedef struct MacCarbSemaphore
{
   pthread_mutex_t mDarkroom;
//   pthread_mutex_t mFoyer; // second lock, to help control starving.
   pthread_cond_t  mCond;
   int count;
};   
   

void * Semaphore::createSemaphore(U32 initialCount)
{
   bool ok;
   MacCarbSemaphore *semaphore = new MacCarbSemaphore();
   ok = pthread_mutex_init(&semaphore->mDarkroom,NULL);
   AssertFatal(ok == 0,"Create semaphore failed at creating mutex mDarkroom.");
//   ok = pthread_mutex_init(&semaphore->mFoyer,NULL);
//   AssertFatal(ok != 0,"Create semaphore failed at creating mutex mFoyer.");
   ok = pthread_cond_init(&semaphore->mCond,NULL);
   AssertFatal(ok == 0,"Create semaphore failed at creating condition mCond.");
   
   semaphore->count = initialCount;
   
   return((void*)semaphore);
}

void Semaphore::destroySemaphore(void * semaphore)
{
   MacCarbSemaphore *sem = static_cast<MacCarbSemaphore*>(semaphore);
   pthread_mutex_destroy(&sem->mDarkroom);
//   pthread_mutex_destroy(sem->mFoyer);
   pthread_cond_destroy(&sem->mCond);
   
   delete sem;
}

bool Semaphore::acquireSemaphore(void * semaphore, bool block)
{ 
   bool ok;
   AssertFatal(semaphore, "Semaphore::acquireSemaphore: invalid semaphore");
   MacCarbSemaphore *sem = static_cast<MacCarbSemaphore*>(semaphore);
   
   ok = pthread_mutex_lock(&sem->mDarkroom);
   AssertFatal(ok == 0,"Mutex Lock failed on mDarkroom in acquireSemaphore().");
   
   if(sem->count <= 0 && !block) {
      ok = pthread_mutex_unlock(&sem->mDarkroom);
      AssertFatal(ok == 0,"Mutex Unlock failed on mDarkroom when not blocking in acquireSemaphore().");
      return false;
   }
   
   while( sem->count <= 0 ) {
      ok = pthread_cond_wait(&sem->mCond, &sem->mDarkroom); // releases mDarkroom while blocked.
      AssertFatal(ok == 0,"Waiting on mCond failed in acquireSemaphore().");
   }
   
   sem->count--;
   
   ok = pthread_mutex_unlock(&sem->mDarkroom);
   AssertFatal(ok == 0,"Mutex Unlock failed on mDarkroom when leaving acquireSemaphore().");
   
   return true;
}

void Semaphore::releaseSemaphore(void * semaphore)
{
   bool ok;
   AssertFatal(semaphore, "Semaphore::releaseSemaphore: invalid semaphore");
   MacCarbSemaphore *sem = static_cast<MacCarbSemaphore*>(semaphore);
   
   ok = pthread_mutex_lock(&sem->mDarkroom);
   AssertFatal(ok == 0,"Mutex Lock failed on mDarkroom in releaseSemaphore().");   
   
   sem->count++;
   if(sem->count > 0) {
      ok = pthread_cond_signal(&sem->mCond);
      AssertFatal(ok == 0,"Signaling mCond failed in releaseSemaphore().");  
   }
   
   ok = pthread_mutex_unlock(&sem->mDarkroom);
    AssertFatal(ok == 0,"Mutex Unlock failed on mDarkroom when leaving releaseSemaphore().");
}
