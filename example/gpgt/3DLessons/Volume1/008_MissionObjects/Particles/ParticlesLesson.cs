//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** LOAD DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./ParticlesLessonDBs.cs");

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
exec("./ParticlesLessonMethods.cs");

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------

function ParticlesLesson::onAdd(%this) 
{
	DefaultLessonPrep();
	LessonMessage("Readying ParticlesLesson ...<br>");
}

function ParticlesLesson::onRemove(%this) 
{
	if(isObject(lessonMap)) lessonMap.delete();
	LessonMessage("... cleaning up ParticlesLesson.<br>");
}

function ParticlesLesson::ExecuteLesson(%this) 
{


	%emitter = new ParticleEmitterNode(Bubbles) 
   {
		position = CalculateObjectDropPosition( North10.getPosition() , "0 0 1" );
		rotation = "1 0 0 0";
		scale = "1 1 1";
		dataBlock = "PENDParticlesLesson";
		emitter = "PEDBubble0";
		velocity = "1";
	};
	LessonGroup.add( %emitter );


	%emitter = new ParticleEmitterNode(Sparks) 
   {
		position = CalculateObjectDropPosition( West10.getPosition() , "0 0 1" );
		rotation = "1 0 0 0";
		scale = "1 1 1";
		dataBlock = "PENDParticlesLesson";
		emitter = "PEDSpark0";
		velocity = "1";
	};
	LessonGroup.add( %emitter );

	%emitter = new ParticleEmitterNode(fire) {
		position = CalculateObjectDropPosition( South10.getPosition() , "-6 0 1" );
		rotation = "1 0 0 0";
		scale = "1 1 1";
		dataBlock = "PENDParticlesLesson";
		emitter = "PEDFire0";
		velocity = "1";
	};
	LessonGroup.add( %emitter );

	%emitter = new ParticleEmitterNode(nicefire) {
		position = CalculateObjectDropPosition( South10.getPosition() , "0 0 1" );
		rotation = "1 0 0 0";
		scale = "1 1 1";
		dataBlock = "PENDParticlesLesson";
		emitter = "PEDNiceFire";
		velocity = "1";
	};
	LessonGroup.add( %emitter );


	%emitter = new ParticleEmitterNode(risingsmoke) {
		position = CalculateObjectDropPosition( East10.getPosition() , "0 -7 1" );
		rotation = "1 0 0 0";
		scale = "1 1 1";
		dataBlock = "PENDParticlesLesson";
		emitter = "PEDRisingSmoke";
		velocity = "1";
	};
	LessonGroup.add( %emitter );

	%emitter = new ParticleEmitterNode(smoke) {
		position = CalculateObjectDropPosition( East10.getPosition() , "0 +5 1" );
		rotation = "1 0 0 0";
		scale = "1 1 1";
		dataBlock = "PENDParticlesLesson";
		emitter = "PEDSmoke0";
		velocity = "1";
	};
	LessonGroup.add( %emitter );

}
