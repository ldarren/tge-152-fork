//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PLATFORMTHREAD_H_
#define _PLATFORMTHREAD_H_

#ifndef _TORQUE_TYPES_H_
#include "platform/types.h"
#endif

typedef void (*ThreadRunFunction)(S32);

class Thread
{
   private:
      static U32 gMainThread;
      
   protected:
      void *      mData;

   public:
      Thread(ThreadRunFunction func = 0, S32 arg = 0, bool start_thread = true);
      virtual ~Thread();

      void start();
      bool join();

      virtual void run(S32 arg = 0);

      bool isAlive();

      static U32 getCurrentThreadId();
      static bool compareThreads(U32 thread1, U32 thread2);
      
      static U32  getMainThread();
      static void setMainThread();
      static bool isMainThread();
};

class PlatformThreadStorage;

/// Platform independent per-thread storage class.
class ThreadStorage
{
   enum
   {
      PlatformThreadStorageStorageSize = 32,
   };

   PlatformThreadStorage *mThreadStorage;
   U8 mStorage[PlatformThreadStorageStorageSize];
public:
   /// ThreadStorage constructor.
   ThreadStorage();
   /// ThreadStorage destructor.
   ~ThreadStorage();

   /// returns the per-thread stored void pointer for this ThreadStorage.  The default value is NULL.
   void *get();
   /// sets the per-thread stored void pointer for this ThreadStorage object.
   void set(void *data);
};


#endif
