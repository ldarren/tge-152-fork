//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platformMacCarb/platformMacCarb.h"
#include "platform/platform.h"
#include "console/console.h"
#include "math/mMath.h"
#include "terrain/blender.h"

extern void mInstallLibrary_C();
extern void mInstallLibrary_Vec();
extern void mInstall_Library_SSE();
extern void mInstallLibrary_ASM();



//--------------------------------------
ConsoleFunction( MathInit, void, 1, 10, "([DETECT|C|VEC|SSE])")
{
   U32 properties = CPU_PROP_C;  // C entensions are always used
   
   if (argc == 1)
   {
         Math::init(0);
         return;
   }
   for (argc--, argv++; argc; argc--, argv++)
   {
      if (dStricmp(*argv, "DETECT") == 0) { 
         Math::init(0);
         return;
      }
      if (dStricmp(*argv, "C") == 0) { 
         properties |= CPU_PROP_C; 
         continue; 
      }
      if (dStricmp(*argv, "VEC") == 0) { 
         properties |= CPU_PROP_ALTIVEC; 
         continue; 
      }
	   if (dStricmp(*argv, "SSE") == 0) { 
		   properties |= CPU_PROP_SSE; 
		   continue; 
	   }
	   Con::printf("Error: MathInit(): ignoring unknown math extension '%s'", *argv);
   }
   Math::init(properties);
}

//------------------------------------------------------------------------------
void Math::init(U32 properties)
{
   if (!properties)
      // detect what's available
      properties = Platform::SystemInfo.processor.properties;  
   else
      // Make sure we're not asking for anything that's not supported
      properties &= Platform::SystemInfo.processor.properties;  

   Con::printf("Math Init:");
   Con::printf("   Installing Standard C extensions");
   mInstallLibrary_C();
   
   if(Platform::SystemInfo.processor.type == CPU_X86Compatible)
   {
      Con::printf("   Installing Assembly extensions");
      mInstallLibrary_ASM();
   }
   
   #if defined(__VEC__)
   Blender::smUseVecBlender = false;
   if (properties & CPU_PROP_ALTIVEC)
   {
      Con::printf("   Installing Altivec extensions");
      mInstallLibrary_Vec();
      Blender::smUseVecBlender = true;
   }
   #endif
   
   if (properties & CPU_PROP_SSE)
   {
	   Con::printf("   Installing SSE extensions");
	   mInstall_Library_SSE();
   }
      
   Con::printf(" ");
} 

//------------------------------------------------------------------------------
F32 Platform::getRandom()
{
   return platState.platRandom.randF();
}


//-----------------------------------------------------------------------------
// Intel asm math
//-----------------------------------------------------------------------------

#if defined(i386)

static S32 m_mulDivS32_ASM(S32 a, S32 b, S32 c)
{  // a * b / c
   S32 r;
   
   __asm__ __volatile__(
      "imul  %2\n"
      "idiv  %3\n"
      : "=a" (r) : "a" (a) , "b" (b) , "c" (c) 
      );
   return r;
}   


static U32 m_mulDivU32_ASM(S32 a, S32 b, U32 c)
{  // a * b / c
   S32 r;
   __asm__ __volatile__(
      "mov   $0, %%edx\n"
      "mul   %2\n"
      "div   %3\n"
      : "=a" (r) : "a" (a) , "b" (b) , "c" (c) 
      );
   return r;
}   
#endif

//------------------------------------------------------------------------------
void mInstallLibrary_ASM()
{
   // stubbed out for ppc builds
#if defined(i386)
   m_mulDivS32              = m_mulDivS32_ASM;
   m_mulDivU32              = m_mulDivU32_ASM;
#endif
}
