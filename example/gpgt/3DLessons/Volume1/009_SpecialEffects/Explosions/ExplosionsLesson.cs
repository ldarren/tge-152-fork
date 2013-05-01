//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** LOAD DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./ExplosionsLessonParticleDBs.cs");
exec("./ExplosionsLessonDebrisDBs.cs");
exec("./ExplosionsLessonDBs.cs");

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
exec("./ExplosionsLessonMethods.cs");

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------

function ExplosionsLesson::onAdd(%lessonObject) 
{
	DefaultLessonPrep();
}

function ExplosionsLesson::onRemove(%lessonObject) 
{
}


function ExplosionsLesson::ExecuteLesson(%lessonObject) 
{
   
   %obj = new StaticShape() 
   {
      dataBlock = "BaseStaticShape";
   };
   
   %obj.position = North20.getPosition();
   DropObject(%obj, "");
   LessonGroup.add( %obj );
   
   %lessonObject.schedule( 0 ,    standAloneExplosion, North20.getPosition() , "FieryExplosion");

   %lessonObject.schedule( 5000 , standAloneExplosion, North20.getPosition() , "ConfettiExplosion");	
}



