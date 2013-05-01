/*
 *  ModernizationKit.h
 *
 *  Copyright (C) 2007 Alex Scarborough
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty.  In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it freely, subject to 
 *  the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 *  4. This software and source code products derived from this software
 *     may only be distributed to persons who own a license for the
 *     Torque Game Engine.
 *
 */

#ifndef _MODERNIZATIONKIT_H_
#define _MODERNIZATIONKIT_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _MKVBOMANAGER_H_
#include "ModernizationKit/mkVBOManager.h"
#endif


struct MK
{
   /// PUBLIC creation and destruction methods ensure that if 
   /// the user somehow tries to create two of use we throw a hissy fit
   static void create();
   static void destroy();
   static void reset();
   static void resurrect();

   static _VBOManager * getVBO();
};

#endif
