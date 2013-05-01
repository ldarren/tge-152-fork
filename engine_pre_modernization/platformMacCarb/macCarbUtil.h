//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _MACCARBUTIL_H_
#define _MACCARBUTIL_H_

#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>

/// Looks in the app's Frameworks folder for a framework and loads it if it finds it.
bool LoadFrameworkBundle(CFStringRef framework, CFBundleRef *bundlePtr);
/// Looks for a framework first in the app then the system, and loads it if it finds it.
bool LoadPrivateFrameworkBundle(CFStringRef framework, CFBundleRef *bundlePtr);

/// Converts a QuickDraw displayID to a Core Graphics displayID.
/// Different Mac APIs need different displayID types. The conversion is trivial
/// on 10.3+, but ugly on 10.2, so we wrap it here.
CGDirectDisplayID MacCarbGetCGDisplayFromQDDisplay(GDHandle hDisplay);

#endif // _MACCARBUTIL_H_