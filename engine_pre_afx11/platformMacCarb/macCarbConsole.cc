//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platformMacCarb/platformMacCarb.h"
#include "platformMacCarb/macCarbConsole.h"
#include "platform/event.h"
#include "platform/gameInterface.h"
#include "platform/platformThread.h"

#include <stdio.h>

// TODO: convert this to use ncurses.

static void _MacCarbConsolePrintPrompt();

MacConsole *gConsole = NULL;

ConsoleFunction(enableWinConsole, void, 2, 2, "(bool enable)")
{
   argc;
   if (gConsole)
      gConsole->enable(dAtob(argv[1]));
}

static void macConsoleConsumer(ConsoleLogEntry::Level, const char *line)
{
   if (gConsole)
      gConsole->processConsoleLine(line);
}

static void macConsoleInputLoopThread(S32 *arg)
{
   if(!gConsole)
      return;
   gConsole->inputLoop();
}

void MacConsole::create()
{
   gConsole = new MacConsole();
}

void MacConsole::destroy()
{
   if (gConsole)
      delete gConsole;
   gConsole = NULL;
}

void MacConsole::enable(bool enabled)
{
   if (gConsole == NULL)
      return;

   bool conIsAlive = consoleThread && consoleThread->isAlive();
   if(consoleThread && !conIsAlive)
      delete consoleThread;
   
   consoleEnabled = enabled;
   if(consoleEnabled)
   {
      if(!conIsAlive)
      {
         printf("Initializing Console...\n");
         new Thread((ThreadRunFunction)macConsoleInputLoopThread,0,true);
         printf("Console Initialized.\n");

         _MacCarbConsolePrintPrompt();
      }
   }
   else
   {
      printf("Deactivating Console.");
   }
}

bool MacConsole::isEnabled()
{
   if ( !gConsole )
      return false;

   return gConsole->consoleEnabled;
}


MacConsole::MacConsole()
{
   consoleEnabled = platState.headless;
   clearInBuf();
   consoleThread = NULL;
   
   Con::addConsumer(macConsoleConsumer);
}

MacConsole::~MacConsole()
{
   Con::removeConsumer(macConsoleConsumer);
}

void MacConsole::processConsoleLine(const char *consoleLine)
{
   if(consoleEnabled)
   {
         printf("%s\n", consoleLine);
   }
}

void MacConsole::clearInBuf()
{
   dMemset(inBuf, 0, MaxConsoleLineSize);
   inBufPos=0;
}

void MacConsole::inputLoop()
{
   Con::printf("Console Input Thread Started");
   unsigned char c;
   while(consoleEnabled)
   {
      c = fgetc(stdin);
      if(feof(stdin) || ferror(stdin))
      {
         clearerr(stdin);
         Platform::sleep(100);
         continue;
      }

      if(c == '\n')
      {
         // exec the line
         dStrcpy(postEvent.data, inBuf);
         postEvent.size = ConsoleEventHeaderSize + dStrlen(inBuf) + 1;
         Con::printf("=> %s",postEvent.data);
         Game->postEvent(postEvent);
         // clear the buffer
         clearInBuf();
         _MacCarbConsolePrintPrompt();
      }
      else
      {
         // add it to the buffer.
         inBuf[inBufPos++] = c;
         // if we're full, clear & warn.
         if(inBufPos >= MaxConsoleLineSize-1)
         {
            clearInBuf();
            Con::warnf("Line too long, discarding this command: %s", inBuf);
         }
      }
   }
   Con::printf("Console Input Thread Stopped");
}

static void _MacCarbConsolePrintPrompt()
{
   printf("%s", Con::getVariable("Con::Prompt"));
}
