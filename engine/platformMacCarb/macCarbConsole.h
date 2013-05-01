//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _MACCARBCONSOLE_H_
#define _MACCARBCONSOLE_H_

#include "console/console.h"
#include "platform/event.h"

class Thread;

class MacConsole
{
private:
   bool consoleEnabled;
   Thread*  consoleThread;
   
   U32   inBufPos;
   char  inBuf[MaxConsoleLineSize];
   ConsoleEvent postEvent;
   
   void clearInBuf();
   
public:
   static void create();
   static void destroy();
   static bool isEnabled();

   MacConsole();
   ~MacConsole();
   void enable(bool);

   void processConsoleLine(const char *consoleLine);
   
   void  inputLoop();

};

extern MacConsole *gConsole;

#endif
