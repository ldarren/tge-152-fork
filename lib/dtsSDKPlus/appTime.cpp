//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma warning(disable : 4786 4018)
#endif

#include "appTime.h"
#include <string>

namespace DTS
{

   // Default time...base pose
   AppTime AppTime::smDefaultTime(0,0);

   // equality tolerance for f64 component
   F64 AppTime::smTOL = 0.0001;

   // when converting to string, display S32? display F32?
   bool AppTime::smPrintInt = false;
   bool AppTime::smPrintFloat = true;

   const char * AppTime::getStr() const
   {
      if (!mBuffer)
         const_cast<char*>(mBuffer) = new char[64];
      if (smPrintInt && smPrintFloat)
         sprintf(mBuffer,"%f:%i",F32(f64),u32);
      else if (smPrintInt)
         sprintf(mBuffer,"%i",u32);
      else if (smPrintFloat)
         sprintf(mBuffer,"%f",F32(f64));
      else
         return "";
      return mBuffer;
   }


}; // namespace DTS
