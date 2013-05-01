//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Note that we do all our memory managament directly against the OS
// facilities. This simplifies things, as it means the profiler is
// not re-entrant during pushes/pops.
//
// (This requirement might not be necessary? Let's question this
//  assumption later... -- BJG)

#if defined(__MACOSX__)
#include <Carbon/Carbon.h>
#endif
#include "platform/profiler.h"
#include "platform/platformMutex.h"
#include "util/safeDelete.h"
#include <stdlib.h> // gotta use malloc and free directly

#ifdef TORQUE_ENABLE_PROFILER

//-----------------------------------------------------------------------------

ThreadStorage gProfilerReentrancyGuard;
volatile bool gGlobalProfilerReentrancyGuard = false;

//-----------------------------------------------------------------------------

#if defined(TORQUE_SUPPORTS_VC_INLINE_X86_ASM)
// platform specific get hires times...
void startHighResolutionTimer(U32 time[2])
{
   //time[0] = Platform::getRealMilliseconds();

   __asm
   {
      push eax
      push edx
      push ecx
      rdtsc
      mov ecx, time
      mov DWORD PTR [ecx], eax
      mov DWORD PTR [ecx + 4], edx
      pop ecx
      pop edx
      pop eax
   }
}

U32 endHighResolutionTimer(U32 time[2])
{
   U32 ticks;
   //ticks = Platform::getRealMilliseconds() - time[0];
   //return ticks;

   __asm
   {
      push  eax
      push  edx
      push  ecx
      //db    0fh, 31h
      rdtsc
      mov   ecx, time
      sub   edx, DWORD PTR [ecx+4]
      sbb   eax, DWORD PTR [ecx]
      mov   DWORD PTR ticks, eax
      pop   ecx
      pop   edx
      pop   eax
   }
   return ticks;
}

#elif defined(TORQUE_SUPPORTS_GCC_INLINE_X86_ASM)

// platform specific get hires times...
void startHighResolutionTimer(U32 time[2])
{
   __asm__ __volatile__(
      "rdtsc\n"
      : "=a" (time[0]), "=d" (time[1])
      );
}

U32 endHighResolutionTimer(U32 time[2])
{
   U32 ticks;
   __asm__ __volatile__(
      "rdtsc\n"
      "sub  0x4(%%ecx),  %%edx\n"
      "sbb  (%%ecx),  %%eax\n"
      : "=a" (ticks) : "c" (time)
      );
   return ticks;
}

#elif defined(TORQUE_OS_MAC)

void startHighResolutionTimer(U32 time[2]) {
   UnsignedWide t;
   Microseconds(&t);
   time[0] = t.lo;
   time[1] = t.hi;
}

U32 endHighResolutionTimer(U32 time[2])  {
   UnsignedWide t;
   Microseconds(&t);
   return t.lo - time[0]; 
   // given that we're returning a 32 bit integer, and this is unsigned subtraction... 
   // it will just wrap around, we don't need the upper word of the time.
   // NOTE: the code assumes that more than 3 hrs will not go by between calls to startHighResolutionTimer() and endHighResolutionTimer().
   // I mean... that damn well better not happen anyway.
}

#else

void startHighResolutionTimer(U32 time[2])
{
}

U32 endHighResolutionTimer(U32 time[2])
{
   return 1;
}

#endif

//-----------------------------------------------------------------------------

ProfilerRoot *ProfilerRoot::smRootList = NULL;
U32           ProfilerRoot::smRootCount = 0;

ProfilerRoot::ProfilerRoot(const char *name)
{
   for(ProfilerRoot *walk = smRootList; walk; walk = walk->mNextRoot)
   {
      if(!dStrcmp(walk->mName, name))
      {
         AssertISV(false, avar("ProfilerRoot::ProfilerRoot - encountered "
                          "duplicate profiler block name '%s'. All block "
                          "names must be unique!", name));
      }
   }

   // Note down our data.
   mName     = name;
   mNameHash = _StringTable::hashString(name);
   mID       = smRootCount++;

   // Link us onto the master list.
   mNextRoot = smRootList;
   smRootList = this;
}

//-----------------------------------------------------------------------------

ProfilerRootData::ProfilerRootData()
{
   mFirstProfilerData = NULL;
   mEnabled = true;
   reset();
}

ProfilerRootData::~ProfilerRootData()
{
}

void ProfilerRootData::reset()
{
   mTotalTime = 0;
   mSubTime   = 0;
   mTotalInvokeCount = 0;
}

//-----------------------------------------------------------------------------

ProfilerData::ProfilerData()
{
   dMemset(this, 0, sizeof(ProfilerData));
}

ProfilerData::~ProfilerData()
{

}

//-----------------------------------------------------------------------------

ProfilerInstance::ProfilerInstance()
{
   // Don't set thread ID here as we might be being allocated
   // anywhere...
   mThreadID = 0;

   // But mutex is OK to allocate now.
   mMutex    = Mutex::createMutex();

   // Set up the root ProfilerData.
   mProfileList = mRootProfilerData = mCurrentProfilerData = (ProfilerData*)malloc(sizeof(ProfilerData));
   constructInPlace(mProfileList);

   // Allocate our ProfilerRootData table.
   mRootTable = NULL;
   mRootCount = 0;
   growRoots(ProfilerRoot::smRootCount);

   // Set up general misc state
   mEnabled          = false;
   mStackDepth       = 0;
   mNextEnable       = false;
   mMaxStackDepth    = MaxStackDepth;
   mDumpToConsole    = false;
   mDumpToFile       = false;
   dMemset(mDumpFileName, 0, sizeof(mDumpFileName));

   mRootCount = ProfilerRoot::smRootCount;
}

ProfilerInstance::~ProfilerInstance()
{
   MutexHandle m;
   m.lock(mMutex);

   free(mRootTable);

   // Walk the profilelist and free things on it.
   ProfilerData *walk = mProfileList, *tmp;
   while(walk)
   {
      tmp = walk;
      walk = walk->mNextProfilerData;

      destructInPlace(tmp);
      free(tmp);
   }

   m.unlock();
   Mutex::destroyMutex(mMutex);
}

void ProfilerInstance::hashPush(ProfilerRoot *pr)
{
   // We need the stack depth tracked at all times so we won't get fscked
   // when the profiler gets turned on in the midst of things... It's also
   // a nice way to validate we aren't in any weird situations vis a vis
   // our profile block boundary setups.

   mStackDepth++;
   AssertFatal(mStackDepth <= mMaxStackDepth,
      "Stack overflow in profiler.  You may have mismatched PROFILE_START and PROFILE_ENDs");

   if(!mEnabled)
      return;

   // Make sure we've got our roots.
   growRoots(ProfilerRoot::smRootCount);

   // Convert from our ProfilerRoot to our ProfilerRootData.
   ProfilerRootData *root = lookup(pr);

   ProfilerData *nextProfiler = NULL;

   // Fast check for cases when we hit a profile block from itself.
   if(!root->mEnabled || mCurrentProfilerData->mOwningRoot == root)
   {
      mCurrentProfilerData->mSubDepth++;
      return;
   }

   // Check to see if we're seeing the same profiler block we saw before,
   // and put in the cache.
   //
   // This gives big wins in cases like:
   // for(S32 i=0; i<1000; i++)
   // {
   //   PROFILE_START(foo); doSomething(); PROFILE_END();
   // }
   if(mCurrentProfilerData->mLastSeenProfiler &&
      mCurrentProfilerData->mLastSeenProfiler->mOwningRoot == root)
      nextProfiler = mCurrentProfilerData->mLastSeenProfiler;

   // Did we get a hit on the cache?
   if(!nextProfiler)
   {
      // Didn't, so let's search for it.

      // first see if it's in the hash table...
      U32 index = pr->mNameHash & (ProfilerData::HashTableSize - 1);
      nextProfiler = mCurrentProfilerData->mChildHash[index];
      while(nextProfiler)
      {
         if(nextProfiler->mOwningRoot == root)
            break;
         nextProfiler = nextProfiler->mNextHash;
      }

      // It wasn't, so let's create a new one.
      if(!nextProfiler)
      {
         // Allocate...
         nextProfiler = (ProfilerData *) malloc(sizeof(ProfilerData));
         constructInPlace(nextProfiler);

         // Link it to its ProfilerRootData
         nextProfiler->mOwningRoot = root;
         nextProfiler->mNextForRoot = root->mFirstProfilerData;
         root->mFirstProfilerData = nextProfiler;

         // Put it on our ProfilerData list.
         nextProfiler->mNextProfilerData = mProfileList;
         mProfileList = nextProfiler;

         // Hash it, too.
         nextProfiler->mNextHash = mCurrentProfilerData->mChildHash[index];
         mCurrentProfilerData->mChildHash[index] = nextProfiler;

         // Fill in parent/child relationships.
         nextProfiler->mParent = mCurrentProfilerData;
         nextProfiler->mNextSibling = mCurrentProfilerData->mFirstChild;
         mCurrentProfilerData->mFirstChild = nextProfiler;
      }
   }

   // Note invocations.
   root->mTotalInvokeCount++;
   nextProfiler->mInvokeCount++;

   // Note our current position, and cache what we last saw.
   mCurrentProfilerData->mLastSeenProfiler = nextProfiler;
   mCurrentProfilerData = nextProfiler;

   // Start the timer.
   startHighResolutionTimer(nextProfiler->mStartTime);
}

void ProfilerInstance::hashPop()
{
   // Update the stack depth.
   mStackDepth--;
   AssertFatal(mStackDepth >= 0, "Stack underflow in profiler.  You may have mismatched PROFILE_START and PROFILE_ENDs");

   if(mEnabled)
   {
      // If we're in a subdepth situation, then just dec it.
      if(mCurrentProfilerData->mSubDepth)
      {
         mCurrentProfilerData->mSubDepth--;
         return;
      }

      // Otherwise, let's end the timer!
      F64 fElapsed = endHighResolutionTimer(mCurrentProfilerData->mStartTime);

      // Update our total time, both for this instance and the root.
      mCurrentProfilerData->mTotalTime              += fElapsed;
      mCurrentProfilerData->mOwningRoot->mTotalTime += fElapsed;

      // Update our parent's subtime as well.
      mCurrentProfilerData->mParent->mSubTime += fElapsed;

      // Parent might be root node, so make sure we don't go into
      // never-never land.
      if(mCurrentProfilerData->mParent->mOwningRoot)
         mCurrentProfilerData->mParent->mOwningRoot->mSubTime += fElapsed;

      // And indicate we're back to our parent.
      mCurrentProfilerData = mCurrentProfilerData->mParent;
   }

   // If we've bottomed out, we're at a frame boundary and can do dumps,
   // and other frame boundary actions with impunity.
   if(mStackDepth == 0)
   {
      MutexHandle m;
      m.lock(mMutex);

      gProfilerReentrancyGuard.set((void*)1);

      // Grab a lock on our mutex so we don't have people changing this
      // stuff when we aren't ready.

      // Do dumps...
      bool doStart = false;

      if(mDumpToConsole || mDumpToFile)
      {
         dump();
         doStart = true;
      }

      // apply the next enable...
      if(!mEnabled && mNextEnable)
         doStart = true;

      mEnabled = mNextEnable;

      gProfilerReentrancyGuard.set((void*)0);

      // Finally, kick off the timer if appropriate.
      if(doStart)
         startHighResolutionTimer(mCurrentProfilerData->mStartTime);
   }
}

void ProfilerInstance::validate()
{
   mEnabled = false;

   // We may be running this in situations where the roots don't match yet,
   // so let's silence this check -- BJG
   //AssertISV(mRootCount == ProfilerRoot::smRootCount, "ProfilerInstance::validate - mismatched root count!");

   // Check by walking the global roots, fetching our instance root, and
   // making sure its children are all properly set up/linked.
   for(ProfilerRoot *prWalk = ProfilerRoot::smRootList; prWalk; prWalk = prWalk->mNextRoot)
   {
      ProfilerRootData *walk = lookup(prWalk);
      for(ProfilerData *dp = walk->mFirstProfilerData; dp; dp = dp->mNextForRoot)
      {
         AssertISV(dp->mOwningRoot == walk, 
            "ProfilerInstance::validate - encountered a ProfilerData under the wrong root!");

         // check if it's in the parent's list...
         ProfilerData *wk;
         for(wk = dp->mParent->mFirstChild; wk; wk = wk->mNextSibling)
            if(wk == dp)
               break;

         AssertISV(wk, 
            "ProfilerInstance::validate - could not find a ProfilerData under its parent's child list!");

         // Check that it's in the right hash.
         for(wk = dp->mParent->mChildHash[prWalk->mNameHash & (ProfilerData::HashTableSize - 1)] ;
            wk; wk = wk->mNextHash)
            if(wk == dp)
               break;

         AssertISV(wk, "ProfilerInstance::validate - could not find a ProfilerDataa in its parent's hash table!");
      }
   }

   mEnabled = true;
}

static S32 QSORT_CALLBACK rootDataCompare(const void *s1, const void *s2)
{
   const ProfilerRootData *r1 = *((ProfilerRootData **) s1);
   const ProfilerRootData *r2 = *((ProfilerRootData **) s2);

   // Because of overflow issues, we have to compare the F64s rather than
   // subtract and return the delta; doing this can result in breakage
   // with numbers in excess of the S32 range. (Note how you commonly
   // experienced weird NS time sorting in the profiler prior to this fix.)
   const F64 time1 = (r1->mTotalTime - r1->mSubTime);
   const F64 time2 = (r2->mTotalTime - r2->mSubTime);

   // Compare and return the right value, goofy. :)
   if(time1 > time2)
      return -1;
   else if(time1 < time2)
      return 1;
   else
      return 0;
}

void ProfilerInstance::printFunc(char *fmt, ...)
{
   U8 buffer[512];

   va_list argptr;

   va_start(argptr, fmt);
   dVsprintf((char*)buffer, 512, fmt, argptr);
   va_end(argptr);

   if(gProfiler->mCurrentDumpIsConsole)
      Con::printf("%s", buffer);

   if(gProfiler->mCurrentDumpIsFile)
      gProfiler->mCurrentDumpStream->writeLine(&buffer[0]);
}

void ProfilerInstance::printSortedRootData(Vector<ProfilerRootData*> &rootVector, F64 totalTime)
{
   printFunc("Profiled blocks ordered by non-sub total time:");
   printFunc("");
   printFunc("%%NSTime  %% Time  Invoke #  Name");
   for(U32 i = 0; i < rootVector.size(); i++)
   {
      printFunc("%7.3f %7.3f %8d  %s",
         F32(100 * (rootVector[i]->mTotalTime - rootVector[i]->mSubTime) / totalTime),
         F32(100 * rootVector[i]->mTotalTime / totalTime),
         rootVector[i]->mTotalInvokeCount,
         rootVector[i]->mRoot->mName);
      rootVector[i]->mTotalInvokeCount = 0;
      rootVector[i]->mTotalTime = 0;
      rootVector[i]->mSubTime = 0;
   }
}

void ProfilerInstance::profilerDataDumpRecurse(ProfilerData *data, char *buffer, U32 bufferLen, F64 totalTime)
{
   if(!data->mOwningRoot)
   {
      printFunc(  "------------- ROOT ----------------");
   }
   else
   {
      printFunc("%7.3f %7.3f %8d  %s%s",
         100 * data->mTotalTime / totalTime,
         100 * (data->mTotalTime - data->mSubTime) / totalTime,
         data->mInvokeCount,
         buffer,
         data->mOwningRoot->mRoot->mName );
   }
   
   // Reset as part of printing (Good idea? -- BJG)
   data->mTotalTime   = 0;
   data->mSubTime     = 0;
   data->mInvokeCount = 0;

   buffer[bufferLen] = ' ';
   buffer[bufferLen+1] = ' ';
   buffer[bufferLen+2] = 0;

   // Sort our children by total time.
   ProfilerData *list = NULL;
   while(data->mFirstChild)
   {
      ProfilerData *ins = data->mFirstChild;
      data->mFirstChild = ins->mNextSibling;
      ProfilerData **walk = &list;
      while(*walk && (*walk)->mTotalTime > ins->mTotalTime)
         walk = &(*walk)->mNextSibling;
      ins->mNextSibling = *walk;
      *walk = ins;
   }

   // And recurse
   data->mFirstChild = list;
   while(list)
   {
      if(list->mInvokeCount)
         profilerDataDumpRecurse(list, buffer, bufferLen + 2, totalTime);
      list = list->mNextSibling;
   }
   buffer[bufferLen] = 0;
}

void ProfilerInstance::dump()
{
   // We assume that we've locked our ProfilerInstance mutex by this point.

   // But we have to get the dump mutex, too, no sense having multiple
   // dumps going at once.
   MutexHandle m;
   m.lock(gProfiler->mDumpMutex);

   // May have some profiled calls... gotta turn em off.
   bool enableSave = mEnabled;
   mEnabled = false;
   mStackDepth++;

   // First, do our search/sort book-keeping.

   // Build a list of all our roots, sorted by non-sub time.
   Vector<ProfilerRootData *> rootVector(ProfilerRoot::smRootCount);
   F64 totalTime = 0;
   for(U32 i=0; i<ProfilerRoot::smRootCount; i++)
   {
      rootVector.push_back(mRootTable + i);
      ProfilerRootData &prd = mRootTable[i];
      totalTime += prd.mTotalTime - prd.mSubTime;
   }
   dQsort((void *)rootVector.address(), rootVector.size(), sizeof(ProfilerRootData *), rootDataCompare);

   // Set up our console status.
   gProfiler->mCurrentDumpIsConsole = mDumpToConsole;

   // Open our stream, if we need it.
   FileStream fws;

   if(mDumpToFile)
   {
      bool success = fws.open(mDumpFileName, FileStream::Write);

      if(success)
      {
         gProfiler->mCurrentDumpIsFile = true;
         gProfiler->mCurrentDumpStream = &fws;
      }
      else
      {
         gProfiler->mCurrentDumpIsFile = false;
      }
   }
   else
   {
      gProfiler->mCurrentDumpIsFile = false;
   }


   // Now, generate our info and blast it out to the appropriate place
   // (ie, console or file.)
   printFunc("Profiler Data Dump:");
   printSortedRootData(rootVector, totalTime);

   printFunc("");
   printFunc("Profiler Data Dump:");

   // Allocate a buffer for indentation use...
   char depthBuffer[MaxStackDepth * 2 + 1];
   depthBuffer[0] = 0;
   
   // Make the recursive call.
   U32 depth = 0;
   profilerDataDumpRecurse(mCurrentProfilerData, depthBuffer, 0, totalTime);

   // Clean up the stream. (Check against the actual write-to-file flag in
   // the profiler, as this won't get set if we failed to open the stream.)
   if(gProfiler->mCurrentDumpIsFile)
      fws.close();

   // Clear our flags.
   mDumpToConsole = false;
   mDumpToFile = false;
   mDumpFileName[0] = '\0';

   // Restore our state.
   mEnabled = enableSave;
   mStackDepth--;
}

void ProfilerInstance::enable(bool enable)
{
   MutexHandle m;
   m.lock(mMutex);

   mNextEnable = enable;

   // Implicit unlock.
}

void ProfilerInstance::dumpToConsole()
{
   MutexHandle m;
   m.lock(mMutex);

   mDumpToConsole = true;

   // If it's a dead thread, then pop never gets called. So we have a quick
   // check here to take care of this take. The pop functionality is under
   // mutex, so we can safely do it here.
   if(mStackDepth == 0)
   {
      mEnabled = false;
      mNextEnable = true;

      // This will dump and reset the flag for us.
      dump();
   }

   // Implicit unlock.
}

void ProfilerInstance::growRoots(U32 newCount)
{
   // If it's the same size or smaller as we've got now, early out.
   if(newCount <= mRootCount)
      return;

   // Otherwise, we have to allocate some new space and update all our
   // pointers. W00t!

   // Get new memory...
   ProfilerRootData *newRoots = (ProfilerRootData*)malloc(sizeof(ProfilerRootData) * newCount);

   // ... then copy our extant data into it...
   if(mRootTable)
      dMemcpy(newRoots, mRootTable, sizeof(ProfilerRootData) * mRootCount);

   // ... and finally initialize the uninitialized entries.
   for(U32 i=mRootCount; i<newCount; i++)
      constructInPlace(&newRoots[i]);

   // Now walk our ProfilerDatas and update their pointers.
   for(ProfilerData *pdWalk = mProfileList; pdWalk; pdWalk = pdWalk->mNextProfilerData)
   {
      // This is silly pointer math, double check it -- BJG.
      if(pdWalk->mOwningRoot)
         pdWalk->mOwningRoot = (pdWalk->mOwningRoot - mRootTable) + newRoots;
   }

   mRootTable = newRoots;
   mRootCount = newCount;

   // Walk the ProfilerRoots and make sure we're pointing to them.
   for(ProfilerRoot *prWalk = ProfilerRoot::smRootList; prWalk; prWalk = prWalk->mNextRoot)
      lookup(prWalk)->mRoot = prWalk;

   // And we're done. Double check. :)
   validate();
}

//-----------------------------------------------------------------------------

/// Pointer to the global profiler.
Profiler *gProfiler = NULL;

static Profiler aProfiler;

Profiler::Profiler()
{
   mInstanceListHead = NULL;
   mDumpMutex = Mutex::createMutex();

   // Singleton magic:
   AssertISV(gProfiler==NULL, "Profiler - a Profiler is already present!");
   gProfiler = this;

}

Profiler::~Profiler()
{
   // Make it so we don't do anything while we're in the destructor.
   gGlobalProfilerReentrancyGuard = true;
   Mutex::destroyMutex(mDumpMutex);

   // Clean up the instance list.
   ProfilerInstance *walk = mInstanceListHead, *tmp;
   while(walk)
   {
      tmp = walk;
      walk = walk->mNextInstance;

      destructInPlace(tmp);
      free(tmp);
   }

   // Singleton magic:
   AssertISV(gProfiler==this, "Profiler - a Profiler other than me is present!");
   gProfiler = NULL;

   gGlobalProfilerReentrancyGuard = false;
}

ProfilerInstance *Profiler::getCurrentInstance()
{
   if(gGlobalProfilerReentrancyGuard || gProfilerReentrancyGuard.get())
      return NULL;

   // Get the instance for this thread, if any.
   ProfilerInstance *pi = (ProfilerInstance*)mCurrentInstance.get();

   // If not, allocate a new one and link it into the
   // list.
   if(!pi)
   {
      gProfilerReentrancyGuard.set((void*)1);

      pi = (ProfilerInstance*)malloc(sizeof(ProfilerInstance));
      constructInPlace(pi);

      pi->mThreadID = Thread::getCurrentThreadId();

      pi->mNextInstance = mInstanceListHead;
      mInstanceListHead = pi;

      mCurrentInstance.set(pi);

      gProfilerReentrancyGuard.set(0);
   }

   return pi;
}

void Profiler::hashPush(ProfilerRoot *root)
{
   ProfilerInstance *pi = getCurrentInstance();
   if(pi) 
      pi->hashPush(root);
}

void Profiler::hashPop()
{
   ProfilerInstance *pi = getCurrentInstance();
   if(pi)
      pi->hashPop();
}

ProfilerInstance *Profiler::getInstanceByID(U32 id)
{
   // Walk the list and get the matching thing.
   ProfilerInstance *walk = mInstanceListHead;

   while(walk)
   {
      if(walk->mThreadID == id)
         return walk;

      walk = walk->mNextInstance;
   }

   return NULL;
}

void Profiler::enable(bool enable)
{
   for(ProfilerInstance *walk = mInstanceListHead; walk; walk = walk->mNextInstance)
      walk->enable(enable);
}

void Profiler::dumpToConsole()
{
   for(ProfilerInstance *walk = mInstanceListHead; walk; walk = walk->mNextInstance)
      walk->dumpToConsole();
}

//-----------------------------------------------------------------------------

ConsoleFunction(profilerEnable, void, 2, 2, "(bool enable) - Turn the profiler on and off.")
{
   if(gProfiler) gProfiler->enable(dAtob(argv[1]));
}

ConsoleFunction(profilerDump, void, 1, 1, "() - Dump all profiled threads' information.")
{
   if(gProfiler) gProfiler->dumpToConsole();
}

#endif
