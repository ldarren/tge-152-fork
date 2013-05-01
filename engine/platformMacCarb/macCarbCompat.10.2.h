//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// macCarbCompat.10.2.h
//-----------------------------------------------------------------------------

#ifndef _MACCARBCOMPAT_10_2_H_
#define _MACCARBCOMPAT_10_2_H_

/// MacCarbInit1020CompatInit sets up function pointers for stubs to functions 
/// that are unavailable in 10.2, if we're running on 10.2
void MacCarbInit1020CompatInit();

// Gestalt values introduced in 10.3+ frameworks.
#if MAC_OS_X_VERSION_MAX_ALLOWED < 1030
#define gestaltCPUG47447   0x0112
#define gestaltCPU970      0x0139
#endif

// Some agl flags introduced in 10.3+
#define AGL_COLOR_FLOAT    58
#define AGL_MULTISAMPLE    59
#define AGL_SUPERSAMPLE    60
#define AGL_SAMPLE_ALPHA   61

// Window transitions, introduced in 10.3+
#define TransitionWindowOptions TransitionWindowOptions_compat
typedef struct TransitionWindowOptions_compat {
  U32       version;
  double    duration;
  void* parentWindowOfSheet;
  void*     userData;
};
//const U32 kEventWindowTransitionCompleted = 89;
//const U32 kWindowFadeTransitionEffect   = 4;
//const U32 kEventParamWindowTransitionAction = 'wtac';
//const U32 typeWindowTransitionAction    = 'wtac';

// replacements for functions not implemented in 10.2:

extern S32 (* TransitionWindowWithOptions_ptr) (void*, U32, U32, const void*, bool, void*);
   
extern bool (* CGCursorIsVisible_ptr) ();
extern U32 (* CGDisplayHideCursor_ptr) (U32);
extern U32 (* CGDisplayShowCursor_ptr) (U32);

#ifndef NO_REDEFINE_10_2_COMPAT
#define CGCursorIsVisible CGCursorIsVisible_ptr
#define CGDisplayHideCursor CGDisplayHideCursor_ptr
#define CGDisplayShowCursor CGDisplayShowCursor_ptr
#define TransitionWindowWithOptions TransitionWindowWithOptions_ptr
#endif

#endif // _MACCARBCOMPAT_10_2_H_