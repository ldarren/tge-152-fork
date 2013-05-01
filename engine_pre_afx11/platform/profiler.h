//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PROFILER_H_
#define _PROFILER_H_

#include "core/torqueConfig.h"

#ifdef TORQUE_ENABLE_PROFILER

   // First, define our macros; that way if we use the profiler in any
   // of the includes we depend on, we'll have them available.
   #define PROFILE_START(name) \
      static ProfilerRoot pdata##name##obj (#name); \
      if(gProfiler) gProfiler->hashPush(& pdata##name##obj )

   #define PROFILE_END() if(gProfiler) gProfiler->hashPop()

#else

   // If profiler is disabled then just stub the blocks out.
   #define PROFILE_START(x)
   #define PROFILE_END()

#endif

// Then include our includes and the real profiler code that needs them.
#ifdef TORQUE_ENABLE_PROFILER

#include "platform/platform.h"
#include "core/stringTable.h"
#include "console/console.h"
#include "core/tVector.h"
#include "core/fileStream.h"
#include "platform/platformThread.h"

struct ProfilerData;
struct ProfilerRoot;
struct ProfilerRootData;
struct ProfilerInstance;

/// Torque includes a full-featured code profiler, which can be used to
/// measure the performance of game code.
///
/// The profiler tracks performance information about a number of blocks in
/// the code base defined by matched sets of PROFILE_START() and
/// PROFILER_END() blocks. Basically, you can use it to see how much time
/// a chunk of code is taking out of your overall execution time.
///
/// The information tracked includes:
///     * Percent of time spent in just this block.
///     * Percent of time spent in this block as well as any other blocks
///       encountered inside of it.
///     * Number of invocations, ie, how many times this block was run.
///
/// The profiler tracks both global statistics for each block, as well as
/// providing hierarchical data about performance (ie, what blocks are
/// called from inside what other blocks, and what time is spent where).
///
/// While this information is not as fine grained as that provided by
/// serious, low-level profiling tools (ie, gprof, VTune, Shark), the
/// overhead is lower. The profiler can be enabled in beta or debug builds
/// of your application, and used to easily dump information about low
/// performance situations. Because it is Torque-aware, it also gives more
/// useful information in some cases than lower level profilers can provide.
///
/// For instance, it can easily show when and how frequently you are
/// allocating/freeing memory, or what the relative breakdown in time is
/// between player update code and physics for other items. It can give
/// useful information for trying to decide if your game is CPU or GPU
/// bound, or if code is being erroneously run too much. (Seeing a helper
/// function is getting run several dozen thousand times more than its
/// parent can be one tip-off. ;)
///
/// <b>How To Use The Profiler</b>
///
/// First, you must #define TORQUE_ENABLE_PROFILER in torqueConfig.h in
/// order to active the profiler. Then you can use it from script. Examples
/// of script use:
///
/// @code
/// // profilerEnable enables or disables profiling.  Data is only gathered
/// // when the profiler is enabled.
/// profilerEnable(bool enable);
/// profilerReset();                                        //resets the data gathered by the profiler
/// profilerDump();                                         //dumps all profiler data to the console
/// profilerDumpToFile(string filename);                    //dumps all profiler data to a given file
/// @endcode
///
/// The C++ code side of the profiler uses pairs of PROFILE_START() and
/// PROFILE_END().
///
/// When using these macros, make sure there is a PROFILE_END() for every
/// PROFILE_START and a PROFILE_START() for every PROFILE_END().  It is fine
/// to nest these macros, however, you must make sure that no matter what
/// execution path the code takes, the PROFILE macros will be balanced. (This
/// usually means having a PROFILE_END() before every return statement in a
/// function, for instance.)
///
/// The profiler can be used to locate areas of code that are slow or should
/// be considered for optimization.  Since it tracks the relative time of
/// execution of that code to the execution of the main loop, it is possible
/// to benchmark any given code to see if changes made will actually
/// improve performance.
///
/// Here are some examples:
/// @code
/// PROFILE_START(TerrainRender);
/// //some code here
/// PROFILE_START(TerrainRenderGridSquare);
/// //some code here
/// PROFILE_END();
/// //possibly some code here
/// PROFILE_END();
///
/// void foo()
/// {
///    PROFILE_START(foo_do);
///    if(doThing())
///    {
///        PROFILE_END();
///        return true;
///    }
///    PROFILE_END();
///    return false;
/// }
/// @endcode
///
/// <b>What Data the Profiler Gathers</b>
///
/// The profiler gathers two general representations of the profiling data.
///
/// First, it gathers per block data. This is data that relates to every
/// single execution of a given block - for instance, foo_do in the
/// above example code has its invocation count and total and non-sub time
/// tracked regardless of where it is called from. This is used to generate
/// the first half of the profiler data dump, the sorted list of what blocks
/// are, overall, taking up the most non-sub time.
///
/// Second, data is gathered per instance of a block. This is the data used
/// to display the nice formatted tree that makes up the second half of a
/// profiler data dump. A single block may have many instances as it is
/// called from different parts of the codebase. For instance, foo_do might
/// get called from the main loop, as well as from the console, making it
/// show up in several places on the hierarchical view:
///
/// @code
///    MainLoop
///      foo_do
///    ConsoleCode
///      foo_do
/// @endcode
///
/// There are three values gathered per block:
///   - Total time. Shown as a percentage of the time period that was profiled,
///     this is the time spent in both this block and all its children.
///   - Non-sub time. This is the amount of time spent ONLY in this block,
///     and not any of its children.
///   - Invocations. The number of times this block was executed.
///
/// <b>How the Profiler Works</b>
///
/// Internally, all profiler state is managed by ProfilerInstance. The
/// gProfiler is just a wrapper around this, responsible for dispatching
/// profiler events (ie, PROFILER_START and PROFILE_END macros, which
/// correspond to calls to Profiler::hashPush and Profiler::hashPop
/// respectively) to the appropriate ProfilerInstance. There is one
/// ProfilerInstance per thread, representing the entire profiler state
/// for that thread.
///
/// ProfilerRoots are allocated statically in each PROFILER_START macro, and
/// are used as "anchors" for a table of ProfilerRootDatas contained in each
/// ProfilerInstance. Each ProfilerRoot is assigned an ID which is used for
/// fast lookups in the ProfilerInstance's ProfilerRootData table. There are
/// potentially many ProfilerRootDatas for a given ProfilerRoot, but only one
/// and exactly one PRD for a PR in a given ProfilerInstance.
///
/// Together, a PR and a PRD represent a "block", i.e., a specific chunk
/// of code begun with a PROFILE_START() and ended with one or more
/// PROFILE_END()s. This is all that is needed to gather the per-block
/// data mentioned in the previous section.
///
/// However, context is often important in analysis, so we also gather
/// hierarchical data, tracking what blocks are called from which other
/// blocks. This allows us to know that, for instance, we only allocate
/// memory once out of every hundred frames in the terrain inner loops,
/// while executing a single console function can involve several frees
/// and allocs. (That is to say, TerrainRender is invoked hundreds of times,
/// while MemoryAlloc and MemoryFree only show up under it once or twice,
/// and ConsoleExec shows up once or twice with MemoryAlloc and MemoryFree
/// being invoked a dozen times each.)
///
/// To acheive this, we make use of the ProfilerData structure. For each
/// "instance" of a block we encounter, we allocate a ProfilerData and place
/// it in a tree structure. A given ProfilerRootData in a ProfilerInstance
/// might have many ProfilerDatas, representing different situations under
/// which it is executed, ie, different blocks that call it.
///
/// So we build a list, and grow a tree from it. By tracking our position
/// in the tree, we can easily build up a consistent view of how our code
/// blocks are called, and how we spend our time in different calling
/// hierarchies.
class Profiler
{
   friend class ProfilerInstance;

   ProfilerInstance *mInstanceListHead;
   ThreadStorage mCurrentInstance;

   /// Get the current ProfilerInstance for the executing thread, or
   /// allocate it if it is unavailable.
   ProfilerInstance *getCurrentInstance();

   /// We only want one thing to dump at a time - imagine the madness
   /// of interleaved output - so we stick a mutex here to manage that, and
   /// some other state information, as it's easy to get to from wherever
   /// we've got the lock.
   void *mDumpMutex;

   bool mCurrentDumpIsConsole;
   bool mCurrentDumpIsFile;
   Stream *mCurrentDumpStream;

public:
   Profiler();
   ~Profiler();

   /// Get a list of the IDs of all the threads we're profiling. Pass these
   /// IDs into the function variants that require an id parameter in order
   /// to control a specific thread's profiling activity.
   void getThreadList(Vector<U32> &ids);

   ProfilerInstance *getInstanceByID(const U32 id);

   void enable(bool enable);
   void dumpToConsole();
   void dumpToFile(const char *fileName);
   void enableMarker(const char *marker, bool enable);
   void reset();

   void hashPush(ProfilerRoot *pr);
   void hashPop();
};

extern Profiler *gProfiler;

/// Helper structure used to store general information about a profiler
/// block.
///
/// Everything that follows are implementation details.
///
/// @see Profiler for information on how to use the profiler.
///
/// We use a slightly indirected scheme to manage the complexities of
/// profiling threads. Basically, the ProfilerRoot structures store the
/// name and other constant, global information about a given profiler
/// block. We walk the list at startup to generate a list at startup
/// which is then used to lookup into a thread-specific table of
/// ProfilerRootDatas. This way we can easily track profile information
/// for each thread without duplication of information or trying to make
/// the core profiling data structures thread-safe via synchronization
/// primitives, which can be horribly slow.
///
/// Profilers must be fast and deterministic, so that they don't impact
/// what they measure. :)
struct ProfilerRoot
{
   /// Name of this profile block.
   const char *mName;

   /// Hash of the profile block name, for fast lookups.
   U32 mNameHash;

   /// Next block in the master list.
   ProfilerRoot *mNextRoot;

   /// Assigned sequentially to map to the thread table.
   /// This allows us to quickly lookup into it. (A hash
   /// would be slow.)
   U32 mID;

   // This structure is exactly 16 bytes. (4 words at 4 bytes each)
   // Following are non-data members:

   /// Head of master list of all profiler blocks.
   static ProfilerRoot *smRootList;

   /// Global count of roots.
   static U32 smRootCount;

   /// Constructor - called by the PROFILE_START macro.
   ProfilerRoot(const char *name);
};

/// Profiler thread table data element. One of these is created for every
/// ProfilerRoot, to store thread specific data. They are indexed by
/// ProfilerRoot::mID.
///
/// @see Profiler for information on how to use the profiler.
struct ProfilerRootData
{
   ProfilerRootData();
   ~ProfilerRootData();

   /// We keep a pointer back to the original root structure so we can do
   /// name lookups and the like.
   ProfilerRoot *mRoot;

   /// Head of linked list of actual ProfilerDatas for this
   /// block.
   ProfilerData *mFirstProfilerData;

   /// Should profile information for this block be gathered?
   bool mEnabled;

   /// @name Data Accumulators
   ///
   ///
   /// @{

   /// Total time spent in all instances of this block.
   F64 mTotalTime;

   /// Total time spent in all child instances of this block, excluding
   /// this block.
   F64 mSubTime;

   /// Total number of times all instances of this block have been invoked.
   U32 mTotalInvokeCount;

   /// @}

   /// Reset all the data accumulators.
   void reset();
};

/// Runtime information on a given instance of a profiler block.
///
/// @see Profiler for information on how to use the profiler.
///
/// Generated on the fly as we build our profiler tree, this represents
/// profiled instances of a block. This enables us to accurately track
/// recursive situations and the like.
///
/// It is organized in the following lists:
///      - Root list. Each root has a list of all its instances associated
///        with it.
///      - ProfilerData list. Each thread keeps a master list of all the
///        ProfilerDatas associated with it.
///      - Hash list. Children of a ProfilerData are kept in a hash indexed
///        by name. Thus a bin list is maintained for each bin of the hash.
///      - Child list. The children of a ProfilerData are all kept in a
///        list for easy traversal.
///
struct ProfilerData
{
   ProfilerData();
   ~ProfilerData();

   /// What ProfilerRootData do we belong to?
   ProfilerRootData *mOwningRoot;

   /// Next ProfilerData for this root.
   ProfilerData *mNextForRoot;

   /// Master list for the profiled thread.
   ProfilerData *mNextProfilerData;

   /// Next ProfilerData in the hash bin.
   ProfilerData *mNextHash;

   /// Who is our parent?
   ProfilerData *mParent;

   /// Next sibling of our parent.
   ProfilerData *mNextSibling;

   /// Our first child.
   ProfilerData *mFirstChild;

   enum {
      HashTableSize = 32,
   };
   ProfilerData *mChildHash[HashTableSize];

   /// For fast lookups, we cache the last seen child. If we see the same
   /// profiler block many times in a row this can save us a lot of
   /// traversal in Profiler::hashPush.
   ProfilerData *mLastSeenProfiler;

   /// The hash of this block's names, kept here for fast lookups.
   U32 mHash;

   /// Number of times we have recursed into this block from itself.
   U32 mSubDepth;

   /// Number of times this block instance has been run.
   U32 mInvokeCount;

   /// The high performance timer needs some memory to store its start time.
   U32 mStartTime[2];

   /// Total time we've spent in this block instance.
   F64 mTotalTime;

   /// Total time we've spent in this block instance's children, excluding
   /// this instance itself.
   F64 mSubTime;
};

/// Manages thread-local profiler state.
///
/// @see Profiler for information on how to use the profiler.
///
/// This is the meat of the profiler; there are one of these for each
/// profiler and they are responsible for tracking all of the state for a
/// thread's profile information.
class ProfilerInstance
{
   friend class Profiler;

   /// @name Dump Code
   ///
   /// Miscellaneous helper functions for dumping the profiler state to
   /// disk or console. We have a unified interface to avoid duplicating
   /// a bunch of complex output code for file/console output.
   ///
   /// If you have a burning need to know what these guys do, just read the
   /// code. It's really pretty straightforward.
   ///
   /// @{

   /// Print data to the appropriate place(s). (Console, stream, or both.)
   static void printFunc(char *fmt, ...);

   /// Print a sorted vector of profiler block data in a nice way.
   static void printSortedRootData(Vector<ProfilerRootData*> &rootVector, F64 totalTime);

   /// Recursively dump part of the profiler data tree.
   static void profilerDataDumpRecurse(ProfilerData *data, char *buffer, U32 bufferLen, F64 totalTime);

   /// @}

   /// We tend to encounter new blocks part-way through execution. This is
   /// due to the nature of static variables (which our global roots are).
   /// They only initialize when they come into scope! So, we have to be able
   /// to grow our local root data table. This ensures it has at least
   /// newCount slows in it, and makes sure everything points to the right
   /// roots.
   void growRoots(U32 newCount);

   enum
   {
      MaxStackDepth = 256,
      DumpFileNameLength = 256
   };

   /// ID of the thread we have profiled data for.
   U32   mThreadID;

   /// Mutex to manage some nasty conditions - we lock on deconstruction,
   /// and also when we're dumping so we don't try to dump from two places
   /// at the same time.
   ///
   /// @note This isn't bullet proof yet on the deconstruct case, but we
   ///       don't actually kill off profiler data so we're ok. :)
   void *mMutex;

   /// Thread-local table of all the ProfilerRootDatas.
   ///
   /// Since we have to keep track of per-block data specific to each
   /// thread (it's a pain to update, nevermind interpret data global to
   /// all threads), we keep a local copy of everything here so we're
   /// essentially independent of the ProfilerRoots scattered throughout
   /// the codebase.
   ProfilerRootData *mRootTable;

   /// Number of roots we have current allocated. Since not all roots are
   /// known (they get initialized as they're visited...), we have to be
   /// able to reallocate our thread-local root table. This isn't much fun
   /// but enables us to be much more robust.
   ///
   /// @see growRoots
   U32 mRootCount;

   /// Our current ProfilerData. This indicates where in the profiler tree
   /// we are at the moment, and is updated on (almost) every push or pop.
   ProfilerData *mCurrentProfilerData;

   /// List of all allocated ProfilerDatas. Important for book-keeping
   /// and update operations.
   ProfilerData *mProfileList;

   /// This is the root ProfilerData in our local tree of profiler data.
   ProfilerData *mRootProfilerData;

   /// Currently enabled?
   bool mEnabled;

   /// Current depth of profiler stack.
   S32  mStackDepth;

   /// Maximum allowed profiler stack depth.
   U32  mMaxStackDepth;

   /// Enable state for next frame.
   bool mNextEnable;

   /// Should we dump to console at next frame?
   bool mDumpToConsole;

   /// Should we dump to file at next frame?
   bool mDumpToFile;

   /// If so, where?
   char mDumpFileName[DumpFileNameLength];

   /// Next allocated ProfilerInstance.
   ProfilerInstance *mNextInstance;

   /// Do a bunch of extra checking to make sure our data structures
   /// are clean.
   ///
   /// @note This is mostly kept around for debug situations, and is not
   ///       normally called.
   void validate();

   /// Look up the ProfilerRootData that corresponds to a given
   /// ProfilerRoot.(
   inline ProfilerRootData *lookup(ProfilerRoot *pr)
   {
      return mRootTable + pr->mID;
   }

   /// Helper function to do the actual dumping. You should have the
   /// instance's mutex before you call this.
   void dump();

public:
   ProfilerInstance();
   ~ProfilerInstance();

   void hashPush(ProfilerRoot *pr);
   void hashPop();

   void enable(bool enable);
   void enableMarker(const char *name, bool enable);
   void reset();
   void dumpToConsole();
   void dumpToFile(const char *fileName);
};

#endif /* TORQUE_ENABLE_PROFILER */

#endif /* _PROFILER_H_ */
