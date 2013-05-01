/*  ModernizationKit.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#include "ModernizationKit/ModernizationKit.h"

_VBOManager *VBOManager = 0;

void MK::create()
{
   AssertFatal(!VBOManager, "Already created MKGFX");
   VBOManager = new _VBOManager();
}

void MK::destroy()
{
   AssertFatal(VBOManager, "Have not created MKGFX");
   delete VBOManager; VBOManager = 0;
}

void MK::reset()
{
   AssertFatal(VBOManager, "Have not created MKGFX");
   if(VBOManager) VBOManager->reset();
}

void MK::resurrect()
{
   AssertFatal(VBOManager, "Have not created MKGFX");
   if(VBOManager) VBOManager->resurrect();
}

_VBOManager * getVBO()
{
	AssertFatal(VBOManager, "call MK::create() first");
	return VBOManager;
}
