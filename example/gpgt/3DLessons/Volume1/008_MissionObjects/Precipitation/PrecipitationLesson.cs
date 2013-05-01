//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** LOAD DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./PrecipitationLessonDBs.cs");


//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
exec("./PrecipitationLessonMethods.cs");

function serverCmdcatsNdogs (%client ) 
{
   PrecipitationLesson.doPrecipitation(1);
}
function serverCmdsnow (%client ) 
{
   PrecipitationLesson.doPrecipitation(2);
}
function serverCmdrain (%client ) 
{
   PrecipitationLesson.doPrecipitation(3);
}


//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------
function PrecipitationLesson::onAdd(%this) 
{
	DefaultLessonPrep();
}

function PrecipitationLesson::onRemove(%this) 
{
	if( isObject($CurrentRain) ) $CurrentRain.delete();
}

function PrecipitationLesson::ExecuteLesson(%this) 
{
	PrecipitationLesson.doPrecipitation(1);
}
