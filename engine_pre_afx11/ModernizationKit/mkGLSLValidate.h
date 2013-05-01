/*  mkGLSLValidate.h
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#ifndef _MKGLSLVALIDATE_H_
#define _MKGLSLVALIDATE_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

/// This is just a handy little class which stores info on the GLSL validation shaders.
struct ValidationShader
{
   StringTableEntry file;
   bool vertex;
   bool pass;
};

/// This simple static struct stores all of the validation shaders.
struct GLSLValidate
{
   static U32 getNumShaders();
   
   static bool mInit;
   
   static Vector<ValidationShader> shaders;
   
   static void init();
   
   static void kill();
};

#endif
   


