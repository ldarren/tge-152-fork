//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** LOAD DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./SoundsLessonDBs.cs");

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
exec("./SoundsLessonMethods.cs");

function serverCmddoMono( %client ) 
{
   SoundsLesson.soundEgg.stopAudio( 0 );
   
   SoundsLesson.soundEgg.schedule( 100 , playAudio, 0, MonoTest );
   
}

function serverCmddoStereo( %client ) 
{
   SoundsLesson.soundEgg.stopAudio( 0 );
   
   SoundsLesson.soundEgg.schedule( 100 , playAudio, 0, StereoTest );
}


//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------
function SoundsLesson::onAdd(%this) 
{
   DefaultLessonPrep();
}

function SoundsLesson::onRemove(%this) 
{
}

function SoundsLesson::ExecuteLesson(%this) 
{
   %this.soundEgg = new StaticShape(leftEgg) 
   {  
      dataBlock = "SoundEgg";
      position  = CalculateObjectDropPosition(North10.getPosition() , "0 0 0" );
      scale     = "0.5 0.5 0.5";
   };
   
   %this.soundEgg.playAudio( 0, MonoTest );
   LessonGroup.add( %this.soundEgg );
   
}
