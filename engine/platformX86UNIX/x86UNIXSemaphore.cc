//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platformX86UNIX/platformX86UNIX.h"
#include "platform/platformSemaphore.h"
// Instead of that mess that was here before, lets use the SDL lib to deal
// with the semaphores.

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

void *Semaphore::createSemaphore(U32 initialCount)
{
  SDL_sem *semaphore;
  semaphore = SDL_CreateSemaphore(initialCount);
  AssertFatal(semaphore, "Semaphore::createSemaphore - Failed.");
  return semaphore;
}

void Semaphore::destroySemaphore(void *semaphore)
{
  AssertFatal(semaphore, "Semaphore::destroySemaphore - Invalid semaphore");
  SDL_DestroySemaphore((SDL_sem *)semaphore);
}

bool Semaphore::acquireSemaphore(void *semaphore, bool block)
{
  AssertFatal(semaphore, "Semaphore::acquireSemaphore - Invalid semaphore");
  if (block)
    {
      if (SDL_SemWait((SDL_sem *)semaphore) < 0)
	AssertFatal(false, "Semaphore::acquieSemaphore - Wait failed.");
      return (true);
    }
  else
    {
      int res = SDL_SemTryWait((SDL_sem *)semaphore);
      if (res < 0)
	AssertFatal(false, "Semaphore::acquireSemaphore - Wait failed.");
      return (res == 0);
    }
}

void Semaphore::releaseSemaphore(void *semaphore)
{
  AssertFatal(semaphore, "Semaphore::releaseSemaphore - Invalid semaphore");
  SDL_SemPost((SDL_sem *)semaphore);
}
