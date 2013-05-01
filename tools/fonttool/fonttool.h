//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _FONTTOOL_H_
#define _FONTTOOL_H_

#include "platform/gameInterface.h"

#define FONTTOOL_DOC_URL "http://tdn.garagegames.com/wiki/TorqueUnicode#Caching.2C_.gft_.2F_.uft"

class FontToolGame : public GameInterface
{
public:
   enum FontToolMode
   {
      FTM_SHOW_USAGE,
      FTM_CACHE_RANGE,
      FTM_CACHE_STRINGS,
      FTM_CACHE_INFO
   };

	S32 main(S32 argc, const char **argv);
};

#endif // _FONTTOOL_H_
