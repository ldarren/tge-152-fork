//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** LOAD DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./DebrisLessonDBs.cs");

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
exec("./DebrisLessonMethods.cs");

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------

function DebrisLesson::onAdd(%this) {
	DefaultLessonPrep();
}

function DebrisLesson::onRemove(%this) {
}


function DebrisLesson::ExecuteLesson(%this) 
{

	standAloneDebris(CalculateObjectDropPosition( North10.getPosition() , "0 0 5" ) , "BaseDebrisExplosion" );

	standAloneDebris(CalculateObjectDropPosition( North30.getPosition() , "0 0 5" ) , "Basis2DDebrisExplosion" );

	standAloneDebris(CalculateObjectDropPosition( East10.getPosition() , "0 0 5" ) , "FastFireDebrisExplosion" );

	standAloneDebris(CalculateObjectDropPosition( East30.getPosition() , "0 0 5" ) , "ParticleTrailingDebrisExplosion" );

	standAloneDebris(CalculateObjectDropPosition( South10.getPosition() , "0 0 5" ) , "Fading3DDebrisExplosion" );

	standAloneDebris(CalculateObjectDropPosition( West10.getPosition() , "0 0 5" ) , "BouncyDebrisExplosion" );

	standAloneDebris(CalculateObjectDropPosition( West30.getPosition() , "0 0 5" ) , "ReplaceWithStaticDebrisExplosion" );

	standAloneDebris(CalculateObjectDropPosition( WestIFLSign.getPosition() , "5 0 0" ) , "SlidingDebrisExplosion" );


}

