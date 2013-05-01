//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "platform/event.h"
#include "platform/gameInterface.h"
#include "core/fileStream.h"
#include "console/console.h"
#include "platform/platformMutex.h"

GameInterface *Game = NULL;
void *gGameEventQueueMutex;

GameInterface::GameInterface()
{
   AssertFatal(Game == NULL, "ERROR: Multiple games declared.");
   Game = this;
   mJournalMode = JournalOff;
   mRunning = true;
   mRequiresRestart = false;
   if(!gGameEventQueueMutex)
      gGameEventQueueMutex = Mutex::createMutex();
   eventQueue = &eventQueue1;
}

int GameInterface::main(int, const char**)
{
   return(0);
}

void GameInterface::textureKill()
{

}

void GameInterface::textureResurrect()
{

}

void GameInterface::refreshWindow()
{

}

static U32 sReentrantCount = 0;

void GameInterface::processEvent(Event *event)
{
   if(!mRunning)
      return;

   if(PlatformAssert::processingAssert()) // ignore any events if an assert dialog is up.
      return;

#ifdef TORQUE_DEBUG
   sReentrantCount++;
   AssertFatal(sReentrantCount == 1, "Error! ProcessEvent is NOT reentrant.");
#endif

   switch(event->type)
   {
      case PacketReceiveEventType:
         processPacketReceiveEvent((PacketReceiveEvent *) event);
         break;
      case MouseMoveEventType:
         processMouseMoveEvent((MouseMoveEvent *) event);
         break;
      case InputEventType:
         processInputEvent((InputEvent *) event);
         break;
      case QuitEventType:
         processQuitEvent();
         break;
      case TimeEventType:
         processTimeEvent((TimeEvent *) event);
         break;
      case ConsoleEventType:
         processConsoleEvent((ConsoleEvent *) event);
         break;
      case ConnectedAcceptEventType:
         processConnectedAcceptEvent( (ConnectedAcceptEvent *) event );
         break;
      case ConnectedReceiveEventType:
         processConnectedReceiveEvent( (ConnectedReceiveEvent *) event );
         break;
      case ConnectedNotifyEventType:
         processConnectedNotifyEvent( (ConnectedNotifyEvent *) event );
         break;
   }

#ifdef TORQUE_DEBUG
   sReentrantCount--;
#endif

}


void GameInterface::processPacketReceiveEvent(PacketReceiveEvent*)
{

}

void GameInterface::processMouseMoveEvent(MouseMoveEvent*)
{

}

void GameInterface::processInputEvent(InputEvent*)
{

}

void GameInterface::processQuitEvent()
{
}

void GameInterface::processTimeEvent(TimeEvent*)
{
}

void GameInterface::processConsoleEvent(ConsoleEvent*)
{

}

void GameInterface::processConnectedAcceptEvent(ConnectedAcceptEvent*)
{

}

void GameInterface::processConnectedReceiveEvent(ConnectedReceiveEvent*)
{

}

void GameInterface::processConnectedNotifyEvent(ConnectedNotifyEvent*)
{

}

struct ReadEvent : public Event
{
   U8 data[3072];
};

FileStream gJournalStream;

void GameInterface::postEvent(Event &event)
{
   if(mJournalMode == JournalPlay && event.type != QuitEventType)
      return;

   // Only one thread can post at a time.
   Mutex::lockMutex(gGameEventQueueMutex);

   if(mJournalMode == JournalSave)
   {
      gJournalStream.write(event.size, &event);
      gJournalStream.flush();
   }
   
   // Create a deep copy of event, and save a pointer to the copy in a vector.
   Event* copy = (Event*)dMalloc(event.size);
   dMemcpy(copy, &event, event.size);
   eventQueue->push_back(copy);
   
   Mutex::unlockMutex(gGameEventQueueMutex);   
}

void GameInterface::processEvents()
{
   // We want to lock the queue when processing as well - don't need
   // anyone putting new events in the middle of this.
   // We double-buffer the event queues so we'll block the other thread(s) for
   // a minimum amount of time.
   Mutex::lockMutex(gGameEventQueueMutex);
      // swap event queue pointers
      Vector<Event*> &fullEventQueue = *eventQueue;
      if(eventQueue == &eventQueue1)
         eventQueue = &eventQueue2;
      else
         eventQueue = &eventQueue1;
   Mutex::unlockMutex(gGameEventQueueMutex);

   // Walk the event queue in fifo order, processing the events, then clear the queue.
   for(int i=0; i < fullEventQueue.size(); i++)
   {
      Game->processEvent(fullEventQueue[i]);
      dFree(fullEventQueue[i]);
   }
   fullEventQueue.clear();

}

void GameInterface::journalProcess()
{
   if(mJournalMode == JournalPlay)
   {
      ReadEvent journalReadEvent;
// used to be:
//      if(gJournalStream.read(&journalReadEvent.type))
//        if(gJournalStream.read(&journalReadEvent.size))
// for proper non-endian stream handling, the read-ins should match the write-out by using bytestreams read:
      if(gJournalStream.read(sizeof(Event), &journalReadEvent))
      {
         if(gJournalStream.read(journalReadEvent.size - sizeof(Event), &journalReadEvent.data))
         {
            if(gJournalStream.getPosition() == gJournalStream.getStreamSize() && mJournalBreak)
               Platform::debugBreak();
            processEvent(&journalReadEvent);
            return;
         }
      }
      // JournalBreak is used for debugging, so halt all game
      // events if we get this far.
      if(mJournalBreak)
         mRunning = false;
      else
         mJournalMode = JournalOff;
   }
}

void GameInterface::saveJournal(const char *fileName)
{
   mJournalMode = JournalSave;
   gJournalStream.open(fileName, FileStream::Write);
}

void GameInterface::playJournal(const char *fileName,bool journalBreak)
{
   mJournalMode = JournalPlay;
   mJournalBreak = journalBreak;
   gJournalStream.open(fileName, FileStream::Read);
}

FileStream *GameInterface::getJournalStream()
{
   return &gJournalStream;
}

void GameInterface::journalRead(U32 *val)
{
   gJournalStream.read(val);
}

void GameInterface::journalWrite(U32 val)
{
   gJournalStream.write(val);
}

void GameInterface::journalRead(U32 size, void *buffer)
{
   gJournalStream.read(size, buffer);
}

void GameInterface::journalWrite(U32 size, const void *buffer)
{
   gJournalStream.write(size, buffer);
}

