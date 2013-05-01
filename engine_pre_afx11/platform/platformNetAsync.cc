//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"

// NetAsync is used by Posix compliant platforms. Since Windows is not Posix
// compliant, NetAsync is not compatible with win32 Torque. Since it sometimes
// gets included by automated build stuff, we're manually snipping it out here.
#if !defined(TORQUE_OS_WIN32)

#include "platform/platformMutex.h"
#include "platform/platformThread.h"
#include "platform/platformNetAsync.h"
#include "console/console.h"

#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

NetAsync gNetAsync;

void* gNetAsyncMutex = NULL;

static void lockNetAsyncMutex()
{
   if(!gNetAsyncMutex)
      gNetAsyncMutex = Mutex::createMutex();

   AssertFatal(gNetAsyncMutex, "Could not create gNetAsyncMutex!");
   
   Mutex::lockMutex(gNetAsyncMutex);
}

static void unlockNetAsyncMutex()
{
   if(!gNetAsyncMutex)
      gNetAsyncMutex = Mutex::createMutex();

   AssertFatal(gNetAsyncMutex, "Could not create gNetAsyncMutex!");
   
   Mutex::unlockMutex(gNetAsyncMutex);
}

// internal structure for storing information about a name lookup request
struct NameLookupRequest
{
      NetSocket sock;
      char remoteAddr[4096];
      char out_h_addr[4096];
      int out_h_length;
      bool complete;

      NameLookupRequest()
      {
         sock = InvalidSocket;
         remoteAddr[0] = 0;
         out_h_addr[0] = 0;
         out_h_length = -1;
         complete = false;
      }
};

void NetAsync::queueLookup(const char* remoteAddr, NetSocket socket)
{
   lockNetAsyncMutex();
   // do we have it already?
   unsigned int i = 0;
   for (i = 0; i < mLookupRequests.size(); ++i)
   {
      if (mLookupRequests[i]->sock == socket)
         // found it.  ignore more than one lookup at a time for a socket.
         break;
   }

   if (i == mLookupRequests.size())
   {
      // not found, so add it
      NameLookupRequest* lookupRequest = new NameLookupRequest();
      lookupRequest->sock = socket;
      dStrncpy(lookupRequest->remoteAddr, remoteAddr, 
               sizeof(lookupRequest->remoteAddr));
      mLookupRequests.push_back(lookupRequest);
   }
   unlockNetAsyncMutex();
}

void NetAsync::run()
{
   if (isRunning())
      return;

   mRunning = true;
   NameLookupRequest* lookupRequest = NULL;

   while (isRunning())
   {
      lookupRequest = NULL;

      // lock 
      lockNetAsyncMutex();
      // if there is a request...
      if (mLookupRequests.size() > 0)
      {
         // assign the first incomplete request
         for (unsigned int i = 0; i < mLookupRequests.size(); ++i)
            if (!mLookupRequests[i]->complete)
               lookupRequest = mLookupRequests[i];
      }

      // unlock so that more requests can be added
      unlockNetAsyncMutex();

      // if we have a lookup request
      if (lookupRequest != NULL)
      {
         // do it
         struct hostent* hostent = gethostbyname(lookupRequest->remoteAddr);
         if (hostent == NULL)
         {
            // oh well!  leave the lookup data unmodified (h_length) should
            // still be -1 from initialization
            lookupRequest->complete = true;
         }
         else
         {
            // copy the stuff we need from the hostent 
            dMemset(lookupRequest->out_h_addr, 0, 
                   sizeof(lookupRequest->out_h_addr));
            dMemcpy(lookupRequest->out_h_addr, hostent->h_addr, hostent->h_length);

            lookupRequest->out_h_length = hostent->h_length;
            lookupRequest->complete = true;
         }
      }
      else
      {
         // no lookup request.  sleep for a bit
         Platform::sleep(500);
      }
   };
}

bool NetAsync::checkLookup(NetSocket socket, char* out_h_addr, 
                           int* out_h_length, int out_h_addr_size)
{
   lockNetAsyncMutex();
   unsigned int i = 0;
   bool found = false;
   // search for the socket
   Vector<NameLookupRequest*>::iterator iter;
   for (iter = mLookupRequests.begin(); 
        iter != mLookupRequests.end(); 
        ++iter)
      // if we found it and it is complete...
      if (socket == (*iter)->sock && (*iter)->complete)
      {
         // copy the lookup data to the callers parameters
         dMemcpy(out_h_addr, (*iter)->out_h_addr, out_h_addr_size);
         *out_h_length = (*iter)->out_h_length;
         found = true;
         break;
      }

   // we found the socket, so we are done with it.  erase.
   if (found)
   {
      delete *iter;
      mLookupRequests.erase(iter);
   }
   unlockNetAsyncMutex();

   return found;
}

// this is called by the pthread module to start the thread
static void StartThreadFunc(S32 nothing)
{
   nothing;

   if (gNetAsync.isRunning())
      return;

   gNetAsync.run();
   return;
}


void NetAsync::startAsync()
{
  if (gNetAsync.isRunning())
     return;

  // create the thread...
   Thread *zThread = new Thread((ThreadRunFunction)StartThreadFunc, 0, true);

  if (!zThread)
     Con::errorf("Error starting net async thread.");
}

void NetAsync::stopAsync()
{
   if (gNetAsync.isRunning())
      gNetAsync.stop();
}

#endif