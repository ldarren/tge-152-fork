//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "core/iTickable.h"

// The statics
U32 ITickable::smLastTick = 0;
U32 ITickable::smLastTime = 0;
U32 ITickable::smLastDelta = 0;

const F32 ITickable::smTickSec = ( F32( ITickable::smTickMs ) / 1000.f );
const U32 ITickable::smTickShift = 5;
const U32 ITickable::smTickMs = ( 1 << smTickShift );
const U32 ITickable::smTickMask = ( smTickMs - 1 );

//------------------------------------------------------------------------------

ITickable::ITickable()
{
   getProcessList().push_back( this );
}

//------------------------------------------------------------------------------

ITickable::~ITickable()
{
   for( ProcessListIterator i = getProcessList().begin(); i != getProcessList().end(); i++ )
   {
      if( (*i) == this )
      {
         getProcessList().erase( i );
         return;
      }
   }
}

//------------------------------------------------------------------------------

Vector<ITickable *>& ITickable::getProcessList()
{
   // This helps to avoid the static initialization order fiasco
   static Vector<ITickable *> smProcessList; ///< List of tick controls
   return smProcessList;
}

//------------------------------------------------------------------------------

bool ITickable::advanceTime( U32 timeDelta )
{
   U32 targetTime = smLastTime + timeDelta;
   U32 targetTick = ( targetTime + smTickMask ) & ~smTickMask;
   U32 tickCount = ( targetTick - smLastTick ) >> smTickShift;

   // If we are going to send a tick, call interpolateTick(0) so that the objects
   // will reset back to their position at the last full tick
   if( smLastDelta && tickCount )
      for( ProcessListIterator i = getProcessList().begin(); i != getProcessList().end(); i++ )
         if( (*i)->isProcessingTicks() )
            (*i)->interpolateTick( 0.f );

   // Advance objects
   if( tickCount )
      for( ; smLastTick != targetTick; smLastTick += smTickMs )
         for( ProcessListIterator i = getProcessList().begin(); i != getProcessList().end(); i++ )
            if( (*i)->isProcessingTicks() )
               (*i)->processTick();

   smLastDelta = ( smTickMs - ( targetTime & smTickMask ) ) & smTickMask;
   F32 dt = smLastDelta / F32( smTickMs );

   // Now interpolate objects that want ticks
   for( ProcessListIterator i = getProcessList().begin(); i != getProcessList().end(); i++ )
      if( (*i)->isProcessingTicks() )
         (*i)->interpolateTick( dt );


   // Inform ALL objects that time was advanced
   dt = F32( timeDelta ) / 1000.f;
   for( ProcessListIterator i = getProcessList().begin(); i != getProcessList().end(); i++ )
      (*i)->advanceTime( dt );

   smLastTime = targetTime;

   return tickCount != 0;
}