//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------
#include "platformMacCarb/platformMacCarb.h"
#include "platformMacCarb/macCarbEvents.h"
#include "platform/platformThread.h"
#include "platform/gameInterface.h"
#include "core/fileio.h"

//-----------------------------------------------------------------------------
// _MacCarbRunTorqueMain() starts the Game.main() loop
//-----------------------------------------------------------------------------
static void _MacCarbRunTorqueMain()
{
   platState.torqueThreadId = Thread::getCurrentThreadId();
   platState.windowSize.set(0,0);
   platState.lastTimeTick = Platform::getRealMilliseconds();
   platState.appReturn = Game->main(platState.argc, platState.argv);
   
   if(!platState.headless)
   {
      HICommand cmd;
      dMemset(&cmd, 0, sizeof(HICommand));
      cmd.commandID = kHICommandQuit;
      ProcessHICommand( &cmd );
   }
}
   
//-----------------------------------------------------------------------------
// Handler stub for bsd signals.
//-----------------------------------------------------------------------------
static void _MacCarbSignalHandler(int )
{
   // we send the torque thread a SIGALRM to wake it up from usleep()
   // when transitioning from background - forground.
}

//-----------------------------------------------------------------------------
//  Thread subclass, for running Torque in multithreaded mode
//-----------------------------------------------------------------------------
class TorqueMainThread : public Thread
{
public:
   TorqueMainThread() : Thread(NULL,0,false) { }

   virtual void run(S32 arg)
   {
      signal(SIGALRM, _MacCarbSignalHandler);
      _MacCarbRunTorqueMain();
   }
};

//-----------------------------------------------------------------------------
//  RunAppEventLoop Callback, for running Torque in single threaded mode
//-----------------------------------------------------------------------------
void _MacCarbRAELCallback(EventLoopTimerRef theTimer, void *userData)
{
   _MacCarbRunTorqueMain();
}

#pragma mark -

//-----------------------------------------------------------------------------
// command line arg processing
//-----------------------------------------------------------------------------
#define KEYISDOWN(key) ((((unsigned char *)currKeyState)[key>>3] >> (key & 7)) & 1)
static bool _MacCarbCheckProcessTxtFileArgs()
{
   // this is yucky, but the easiest way to ignore the cmd line args:
   KeyMap currKeyState;
   GetKeys(currKeyState);
   if (KEYISDOWN(0x38)) // check shift key -- actually LShift.
      return false;
   else
      return true;
}

//-----------------------------------------------------------------------------
static void _MacCarbGetTxtFileArgs(int &argc, char** argv, int maxargc)
{
   argc = 0;
   
   const U32 kMaxTextLen = 2048; // arbitrary
   U32 textLen;
   char* text = new char[kMaxTextLen];   

   // open the file, kick out if we can't
   File cmdfile;
   File::Status err = cmdfile.open("maccmdline.txt", cmdfile.Read);
   if(err != File::Ok)
      return;
   
   // read in the first kMaxTextLen bytes, kick out if we get errors or no data
   err = cmdfile.read(kMaxTextLen-1, text, &textLen);
   if(!((err == File::Ok || err == File::EOS) || textLen > 0))
   {
      cmdfile.close();
      return;
   }
   
   // null terminate
   text[textLen++] = '\0';
   // truncate to the 1st line of the file
   for(int i = 0; i < textLen; i++)
   {
      if( text[i] == '\n' || text[i] == '\r' )
      {
         text[i] = '\0';
         textLen = i+1;
         break;
      }
   }

   // tokenize the args with nulls, save them in argv, count them in argc
   char* tok;
   for(tok = dStrtok(text, " "); tok && argc < maxargc; tok = dStrtok(NULL, " "))
   {
      argv[argc++] = tok;
   }
}

//-----------------------------------------------------------------------------
static void _MacCarbFilterCmdLineArgs( int &argc, char** argv)
{
   // MacOSX gui apps get at least 2 args: the full path to their binary, 
   // and a process serial number, formed something like: "-psn_0_123456".
   // Torque doesnt want to see the psn arg, so we strip it out.
   int j = 0;
   for(int i = 0; i < argc; i++)
   {
      if(dStrncmp(argv[i], "-psn", 4) != 0)
         argv[j++] = argv[i];

      if(dStrncmp(argv[i], "-headless", 9) == 0)
      {
         printf("entering headless mode\n");
         platState.headless = true;
      }
   }
   argc = j;
}

#pragma mark -

//-----------------------------------------------------------------------------
// main() - the real one - this is the actual program entry point.
//-----------------------------------------------------------------------------
S32 main(S32 argc, const char **argv)
{
   const int kMaxCmdlineArgs = 32; // arbitrary

   // get the actual command line args
   S32   newArgc = argc;
   char* newArgv[kMaxCmdlineArgs];
   for(int i=0; i < argc && i < kMaxCmdlineArgs; i++)
      newArgv[i] = argv[i];

   if( _MacCarbCheckProcessTxtFileArgs() )
   {
      // get the text file args
      S32 textArgc;
      char* textArgv[kMaxCmdlineArgs];
      _MacCarbGetTxtFileArgs(textArgc, textArgv, kMaxCmdlineArgs);
      
      // merge them
      int i=0;
      while(i < textArgc && newArgc < kMaxCmdlineArgs)
         newArgv[newArgc++] = textArgv[i++];
   }
   
   // filter them
   _MacCarbFilterCmdLineArgs( newArgc, newArgv);
   
   // store them in platState
   platState.argc = newArgc;
   platState.argv = newArgv;
    
   MacCarbInit1020CompatInit();

   // Headless mode is for sitations where torque must run as a command line
   // tool, without a connection to the window server. Any windowing or event 
   // calls may crash the app if there is no window server, so we avoid them
   // in headless mode.
   if(!platState.headless)
   {
      InitCursor();
      
      FlushEvents( everyEvent, 0 );
      SetEventMask(everyEvent);

      // push us to the front, just to be sure
      ProcessSerialNumber psn = { 0, kCurrentProcess };
      SetFrontProcess(&psn);
   }
   
   // save away OS version info into platState.
   Gestalt(gestaltSystemVersion, (SInt32 *) &(platState.osVersion));

   // Update the current working directory.
   Platform::getWorkingDirectory();
    
   // now, we prepare to hand off execution to torque & macosx.
   platState.appReturn = 0;
   platState.firstThreadId = Thread::getCurrentThreadId();

#if !defined(TORQUE_MULTITHREAD)
   // Install a one-shot timer to run the game, then call RAEL to install
   // the default application handler (which can't be called directly).
   EventLoopTimerRef timer;
   InstallEventLoopTimer(GetCurrentEventLoop(), 0, 0, 
                     NewEventLoopTimerUPP(_MacCarbRAELCallback), NULL, &timer);
   RunApplicationEventLoop();
#else
   // Put the Torque application loop in one thread, and the event listener loop
   // in the other thread. The event loop must use the process's initial thread.

   // We need to cache a ref to the main event queue because GetMainEventQueue
   // is not thread safe pre 10.4 . 
   platState.mainEventQueue = GetMainEventQueue();

   // We need to install event handlers for interthread communication.
   // Events and some system calls must happen in the process's initial thread.
   MacCarbInstallTorqueCarbonEventHandlers();
   
   TorqueMainThread mainLoop;
   mainLoop.start();
   
   if(!platState.headless)
   {
      //printf("starting RAEL\n");
      RunApplicationEventLoop();
   }
   //printf("trying to join main loop...\n");
   mainLoop.join();
   //printf("main loop joined.\n");
#endif   
   InitCursor(); // don't leave it in a screwy state...
   
   //printf("exiting...\n");   
   return(platState.appReturn);
   
}
