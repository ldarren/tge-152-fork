//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
function ExplosionsLesson::standAloneExplosion(  %theLesson , %position , %datablockName) {

	%explosion = new Explosion() {
		datablock = %datablockName;
		position  = CalculateObjectDropPosition( %position , "0 0 1" );
	};

	//
	// Note: We don't really need to keep track of this as explosions 'auto-delete', but
	// if you open the mission editor, you can see how this will appear then be automatically
	// removed when the explosion is complete.
	//
	LessonGroup.add( %explosion );  

	%theLesson.schedule( 10000 , standAloneExplosion, %position , %datablockName);
}

