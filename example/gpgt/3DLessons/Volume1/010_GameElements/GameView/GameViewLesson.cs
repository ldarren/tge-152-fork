//------------------------------------------------------
// Copyright 2001-2005, Hall Of Worlds, LLC
// Portions Copyright 2000-2005, GarageGames.com, Inc.
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** LOAD DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./GameViewLessonDBs.cs");

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
exec("./GameViewLessonMethods.cs");

function serverCmdswitchCameraDB( %client ) 
{
   GameViewLesson.currentCamDB++;
	  
	  if(GameViewLesson.currentCamDB >= GameViewLesson.camDBCount) 
	  {
	     GameViewLesson.currentCamDB = 0;
	  }

   %script = "tok.camera.setDatablock(testCamera" @ GameViewLesson.currentCamDB @ ");";
	  
	  ClientGroup.forEachStmt( %script , tok , true );

}

function serverCmdswitchPlayerDB( %client ) 
{
   GameViewLesson.currentAvatarDB++;
	  
	  if(GameViewLesson.currentAvatarDB >= GameViewLesson.avatarDBCount) 
	  {
	     GameViewLesson.currentAvatarDB = 0;	     
	  }

   %script = "tok.player.setDatablock(testAvatar" @ GameViewLesson.currentAvatarDB @ ");";
	  
	  ClientGroup.forEachStmt( %script , tok , true );
	  
}

function serverCmdincreaseFOV( %client ) 
{
   GameViewLesson.currentFOV += 15;
   
   %maxFOV = %client.getControlObject().getDatablock().cameraMaxFOV;
   
   if( GameViewLesson.currentFOV >= %maxFOV )
   {
      GameViewLesson.currentFOV = %maxFOV;
   }

   if( GameViewLesson.currentFOV > 140.0 )
   {
      GameViewLesson.currentFOV = 140.0;
   }
  
   $Pref::player::CurrentFOV = GameViewLesson.currentFOV;
   
   echo("\c4increase FOV == ", $Pref::player::CurrentFOV);
   
   setFOV( GameViewLesson.currentFOV );
}

function serverCmddecreaseFOV( %client ) 
{
   GameViewLesson.currentFOV -= 15;
   
   %minFOV = %client.getControlObject().getDatablock().cameraMinFOV;
   
   if( GameViewLesson.currentFOV <= %minFOV )  
   {
      GameViewLesson.currentFOV = %minFOV;
   }

   if( GameViewLesson.currentFOV < 1.0 )
   {
      GameViewLesson.currentFOV = 1.0;
   }
     
   $Pref::player::CurrentFOV = GameViewLesson.currentFOV;

   echo("\c5decrease FOV == ", $Pref::player::CurrentFOV);
   
   setFOV( GameViewLesson.currentFOV );
}

function serverCmdincreaseZoomSpeed( %client ) 
{
   GameViewLesson.currentZoomSpeed -= 100;
   
   if(GameViewLesson.currentZoomSpeed < 0)
   {
      GameViewLesson.currentZoomSpeed = 0;
   }
   
   $pref::Player::zoomSpeed = GameViewLesson.currentZoomSpeed;

   setZoomSpeed( GameViewLesson.currentZoomSpeed );
}

function serverCmddecreaseZoomSpeed( %client ) 
{
   GameViewLesson.currentZoomSpeed += 100;
   
   if(GameViewLesson.currentZoomSpeed > 4000)
   {
      GameViewLesson.currentZoomSpeed = 4000;
   }
   
   $pref::Player::zoomSpeed = GameViewLesson.currentZoomSpeed;

   setZoomSpeed( GameViewLesson.currentZoomSpeed );
}


//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------

function GameViewLesson::onAdd( %theLesson ) {
	DefaultLessonPrep();

   %theLesson.camDBCount = 6;
   %theLesson.currentCamDB = 0;

   %theLesson.avatarDBCount = 10;
   %theLesson.currentAvatarDB = 0;

   %theLesson.currentFOV = 90;
   %theLesson.currentZoomSpeed = 500;
	
   %script = "tok.camera.setDatablock(testCamera" @ %theLesson.currentCamDB @ ");";
	  
   ClientGroup.forEachStmt( %script , tok , true );

   %script = "tok.player.setDatablock(testAvatar" @ %theLesson.currentAvatarDB @ ");";
	  
   ClientGroup.forEachStmt( %script , tok , true );
	
}

function GameViewLesson::onRemove(%theLesson) 
{
   
   $pref::Player::zoomSpeed = 0;
   $Pref::player::CurrentFOV = 45;
   setFOV( 45 );
   setZoomSpeed( 0 );
}


function GameViewLesson::ExecuteLesson(%theLesson) 
{
	GameView.help();

	%theLesson.currentFOV = 90;
	%theLesson.currentZoomSpeed = 500;
}

 


