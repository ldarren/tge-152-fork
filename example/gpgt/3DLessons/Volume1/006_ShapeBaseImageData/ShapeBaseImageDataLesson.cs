//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** LOAD DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./ShapeBaseImageDataLessonDBs.cs");

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
exec("./ShapeBaseImageDataLessonMethods.cs");

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------

function ShapeBaseImageDataLesson::onAdd(%this) {
	DefaultLessonPrep();
}


function ShapeBaseImageDataLesson::ExecuteLesson(%this) {
	
	%obj = new StaticShape() 
	{  
		dataBlock	= "trafficLightPole";
		position	= North10.getPosition();
		rotation	= "0 0 1 180";
	};
	DropObject(%obj, "");
	LessonGroup.add( %obj );

}
