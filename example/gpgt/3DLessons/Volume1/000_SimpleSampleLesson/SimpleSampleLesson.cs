//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

//------------------------------------------------------
// ****** LOAD/DEFINE DATABLOCKS FOR LESSON
//------------------------------------------------------

//None.

//------------------------------------------------------
// ****** LOAD/DEFINE FUNCTIONS and METHODS FOR LESSON
//------------------------------------------------------
function dummyItem::onPickup( %pickupDB , %pickupObj , %ownerObj )
{
   return false;
}

function dummyItem::onCollision( %colliderDB , %colliderObj , %collidedObj, %vec, %speed)
{
   return false;
}

function serverCmdbumpBlock( %client ) 
{
   echo("\c3 Client ", %client , " told me to bump the block" );
   
   LessonGroup.dummyBlock.applyImpulse( LessonGroup.dummyBlock.getWorldBoxCenter() , "0 0 200");
}

//------------------------------------------------------
// ****** DEFINE THE *REQUIRED* LESSON METHODS 
//------------------------------------------------------
function SimpleSampleLesson::onAdd(%this) 
{
	DefaultLessonPrep();
}

function SimpleSampleLesson::onRemove(%this) 
{	
}

function SimpleSampleLesson::ExecuteLesson(%this) 
{
   
   %dummyBlock = new Item( dummyItem ) {  
      dataBlock = "BaseItem";
      position  = CalculateObjectDropPosition(North10.getPosition() , "-2 1 0" );
      scale     = "2 2 2";
   };
   
   LessonGroup.add( %dummyBlock );
   
   LessonGroup.dummyBlock = %dummyBlock;
   
} // SimpleSampleLesson::ExecuteLesson()

