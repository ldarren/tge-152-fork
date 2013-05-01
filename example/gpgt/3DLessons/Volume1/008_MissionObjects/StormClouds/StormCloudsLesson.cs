//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
function serverCmdtoggleClouds( %client )  
{
   if(StormCloudsLesson.CloudsNextToggle $= "fadein") 
   {
      StormCloudsLesson.CloudsNextToggle = "fadeout";
      Sky.stormClouds(  true , 1.5 );
   } 
   else 
   {
      StormCloudsLesson.CloudsNextToggle = "fadein";
      Sky.stormClouds( false , 1.5 );
	}
}


//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------
function StormCloudsLesson::onAdd(%this) 
{
	DefaultLessonPrep();
}

function StormCloudsLesson::onRemove(%this) 
{
	// Fade clouds in completely (immediatly)
	Sky.stormClouds( 1 , 0.01 );
}

function StormCloudsLesson::ExecuteLesson(%this) 
{
   StormCloudsLesson.CloudsNextToggle = "fadeout";
}
