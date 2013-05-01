/*  MKInit.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

//#include "ModernizationKit/UnitTests/mkTesting.h"

#include "ModernizationKit/MKInit.h"
#include "ModernizationKit/ModernizationKit.h"
#include "ModernizationKit/mkFluid.h"

bool didInit = false;

void MKInit()
{
   AssertFatal( !didInit, "Already initialized the Modernization Kit!");
   _ShaderManager::create();
   _VBOManager::create();
   _MaterialManager::create();
   _RenderTextureManager::create();
   _DRL::create();
   _MKGFX::create();
   
   // Set mkFluid prefs
   mkFluid::setGlobalReflectionSize(getNextPow2(Con::getIntVariable("$pref::Water::reflectionSize", 1)));
   mkFluid::enableGlobalRefraction(Con::getBoolVariable("$pref::Water::refract", false));
   
   #ifdef TORQUE_OS_MAC_OSX
   #ifdef TORQUE_DEBUG
   
   //bool testsGood = mkRunTests();
   
   #endif
   #endif
   
   didInit = true;
}

void MKDestroy()
{
   AssertFatal(didInit, "MK not initialized (already destroyed?)");
   _ShaderManager::destroy();
   _VBOManager::destroy();
   _MaterialManager::destroy();
   _RenderTextureManager::destroy();
   _DRL::destroy();
   _MKGFX::destroy();
   didInit = false;
}

void MKReset()
{
   if(ShaderManager)
      ShaderManager->reset();
   
   if(VBOManager)
      VBOManager->reset();
      
   if(RenderTextureManager)
      RenderTextureManager->reset();
}

void MKResurrect()
{
   if(ShaderManager)
      ShaderManager->resurrect();
   
   if(VBOManager)
      VBOManager->resurrect();
   
   if(RenderTextureManager)
      RenderTextureManager->resurrect();
}


