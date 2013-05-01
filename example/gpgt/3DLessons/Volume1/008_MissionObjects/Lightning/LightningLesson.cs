//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** LOAD DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./LightningLessonDBs.cs");

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
function serverCmdrandomStrike( %client ) 
{
   if( ( isObject(LightningLesson.CurrentLightning) )  &&
       ( "Lightning" $= LightningLesson.CurrentLightning.getClassName() ) )
   {
      LightningLesson.strikeRandomPoint();
   }
}

function serverCmdDoGeneratedLightning( %client ) 
{
   if( isObject(LightningLesson.CurrentLightning) ) 
   {
      LightningLesson.CurrentLightning.delete();
   }

   LightningLesson.CurrentLightning = new Lightning() {
      position = CalculateObjectDropPosition( North40.getPosition() , "0 0 180");
      scale = "100 100 500";
      dataBlock = "GeneratedLightningExample";
      strikesPerMinute = "90";
      strikeWidth = "0.25";
      chanceToHitTarget = "100";
      strikeRadius = "25";
      boltStartRadius = "100";
      color = "1.000000 1.000000 1.000000 1.000000";
      fadeColor = "0.100000 0.100000 1.000000 1.000000";
      useFog = "0";
   };   
}

function serverCmdDoTexturedLightning( %client ) 
{
   if( isObject(LightningLesson.CurrentLightning) ) 
   {
      LightningLesson.CurrentLightning.delete();
   }

   LightningLesson.CurrentLightning = new WeatherLightning() {
      position = CalculateObjectDropPosition( North40.getPosition() , "0 0 250");
      scale = "200 200 500";
      dataBlock = "TexturedLightningExample";
      strikesPerMinute = "30";
   };   
}

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------
function LightningLesson::onAdd(%this) 
{
	DefaultLessonPrep();
}

function LightningLesson::onRemove(%this) 
{	
   if( isObject(LightningLesson.CurrentLightning) ) 
   {
      LightningLesson.CurrentLightning.delete();
   }
}

function LightningLesson::ExecuteLesson(%this) 
{
   serverCmdDoGeneratedLightning( 0 );
}
