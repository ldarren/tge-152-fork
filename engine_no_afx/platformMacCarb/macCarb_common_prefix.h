//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//------------------------------
//maccarb_common_prefix.h
//------------------------------

#define TORQUE_OS_MAC_CARB       1     // always defined right now...

#if defined(TORQUE_OS_MAC_OSX)
//#define Z_PREFIX  // OSX comes with zlib, so generate unique symbols.
#endif

// defines for the mac headers to activate proper Carbon codepaths.
#define TARGET_API_MAC_CARBON    1   // apple carbon header flag to take right defpaths.
//#define OTCARBONAPPLICATION      1   // means we can use the old-style funcnames

// determine the OS version we're building on...
//  MAC_OS_X_VERSION_MAX_ALLOWED will have the local OSX version,
//  or it will have the version of OSX for the sdk we're cross compiling with.
#include <AvailabilityMacros.h>

// Pasteboards were introduced in 10.3, and are not available before 10.3
// QDGetCGDirectDisplayID was introduced in 10.3, but there is a manual workaround.
// There's a bug in some versions of Xcode, such that MAC_OS_X_VERSION_MAX_ALLOWED
// is not set correctly, and so we cannot properly use the weak-linking feature.
#if MAC_OS_X_VERSION_MIN_REQUIRED >= 1030
#define TORQUE_MAC_HAS_PASTEBOARD
#define TORQUE_MAC_HAS_QDGETCGDIRECTDISPLAYID
#endif

// Some features we're currently using were introduced in 10.3, or 10.4.
// We'll define them, or provide stubs for them, in the case that we are on 10.2
#if MAC_OS_X_VERSION_MIN_REQUIRED <= 1020
#include "platformMacCarb/macCarbCompat.10.2.h"
#else
#define MacCarbInit1020CompatInit() // stub it out
#endif

// Some features we're currently using were introduced in 10.4.
// for now, it's just a few values, so we define them here.
#if MAC_OS_X_VERSION_MIN_REQUIRED <= 1030
#define gestaltCPU970FX    0x013C
#define gestaltCPUPentium4 'i5iv'
#define CPU_TYPE_X86       ((cpu_type_t) 7)
#define CPU_ARCH_ABI64     0x01000000  /* 64 bit ABI */
#define CPU_TYPE_X86_64    (CPU_TYPE_X86 | CPU_ARCH_ABI64)
#define CPU_TYPE_POWERPC64 (CPU_TYPE_POWERPC | CPU_ARCH_ABI64)

#endif
