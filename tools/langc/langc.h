//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _LANGC_H_
#define _LANGC_H_

#include "platform/gameInterface.h"

#define I18N_DOC_URL	"http://tdn.garagegames.com/wiki/TorqueLocalization"

class LangCGame : public GameInterface
{
public:
	S32 main(S32 argc, const char **argv);
};

#endif // _LANGC_H_
