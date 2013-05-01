//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** DEFINE DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------


function TSStaticLesson::onAdd(%this) {
	DefaultLessonPrep();
	LessonMessage("Readying TSStatic lesson...<br>");
}

function TSStaticLesson::onRemove(%this) {
	if(isObject(lessonMap)) lessonMap.delete();
	LessonMessage("... cleaning up TSStatic.<br>");
}

function TSStaticLesson::help(%this) {
	LessonMessage("Lesson help:<br>");
	LessonMessage("      [h] - help<br>");
}


function TSStaticLesson::ExecuteLesson(%this) {

	//
	// Map any keys required for this lesson in an action map
	// named: lessonMap.  It will automatically be unloaded
	// on game exit and when the next lesson is loaded.
	//
	new ActionMap(lessonMap);
	lessonMap.bindCmd(keyboard, "h", "", "TSStaticLesson.help();");
	lessonMap.push();


	//
	// A Plain EGG
	//

	%obj = new TSStatic() {
		position = CalculateObjectDropPosition( North10.getPosition() , "0 0 0" );
		rotation = "1 0 0 0";
		scale = "1 1 1";
		shapeName = "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";
	};

	LessonGroup.add( %obj );

	//
	// A 'Hovering' EGG
	//

	%obj = new TSStatic() {
		position = CalculateObjectDropPosition( West10.getPosition() , "0 0 2" );
		rotation = "1 0 0 0";
		scale = "1 1 1";
		shapeName = "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";
	};

	LessonGroup.add( %obj );

	//
	// A Scaled and Rotated EGG
	//

	%obj = new TSStatic() {
		position = CalculateObjectDropPosition( South10.getPosition() , "0 0 0" );
		rotation = "-1 0 0 45";
		scale = "0.5 0.5 2";
		shapeName = "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";
	};

	LessonGroup.add( %obj );


	LessonMessage("TSStatic started...<br>");	

	TSStaticLesson.help();
}

