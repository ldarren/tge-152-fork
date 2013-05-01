//------------------------------------------------------
// Copyright 2000-2005, GarageGames.com, Inc.
// Written, modfied, or otherwise interpreted by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Loading MazeRunner Player Datablock Definitions and Scripts ---------");

function rldplayer() {
   exec("./mazerunnerplayer.cs");
}

//-------------------------------------------------------------------------
//             MazeRunner Datablock Definition
//-------------------------------------------------------------------------
datablock PlayerData( MazeRunner )
{
    shapeFile            = "~/data/player/MazeRunner.dts";
    category             = "Bipeds";

    pickupRadius         = 0.75;

    boundingBox          = "1.6 1.6 2.3"; 
    useEyePoint          = false;   
    pickupRadius         = 0.75;
    runForce             = 45;
    jumpForce            = 70 ; 
    jumpDelay            = 0; 
    drag                 = 0;
    mass                 = 1;

    invincible           = true;

    minJumpSpeed                 = 0;     
    maxJumpSpeed                 = 0;     

    maxBackwardSpeed     = 10;      // Maximum backward velocity (on ground)
    maxForwardSpeed      = 14;      // Maximum forward velocity (on ground)
    maxSideSpeed         = 12;      // Maximum sideways velocity (on ground)

    horizMaxSpeed                = 68;   // Maximum horizontal component of velocity (general)
    horizResistSpeed             = 33;   // Horizontal component of velocity (general) at which drag activates
    horizResistFactor            = 0.35; // Resistance multiplier

    upMaxSpeed                   = 100;  // Maximum vertical component of velocity (general)
    upResistFactor               = 10;   // Vertical component of velocity (general) at which drag activates
    upResistSpeed                = 1;    // Resistance multiplier

    maxStepHeight                = 0.75; // Max height player can step up (0.75m ~= 2.5ft)

    groundImpactMinSpeed  = 1000; 
    minImpactSpeed        = 1000;

    recoverDelay                 = 0;    // Delays motion for N ticks after a fall
    recoverRunForceScale         = 0.0;  // Multiplier modifying run force during recovery period

    renderFirstPerson    = false; 

    observeThroughObject = true; 
    firstPersonOnly      = false;

    cameraDefaultFov     = 90.0;
    cameraMinFov         = 45.0;
    cameraMaxFov         = 120.0;
    cameraMinDist        = 18.0;
    cameraMaxDist        = 25.0;
    minLookAngle         = -1.73;
    maxLookAngle         = 1.73; 
    maxFreelookAngle     = 3.14159;	
                                    
    useEyePoint             = false; 

    dustEmitter = "";



   maxTimeScale    = 1.5;  // Maximum time scaling of action animations 

};

datablock TSShapeConstructor(MazeRunnerDts)
{
   baseShape = "~/data/player/mazerunner.dts";
   sequence0 = "~/data/player/sequence_root.dsq root";
   sequence1 = "~/data/player/sequence_run.dsq run";
   sequence2 = "~/data/player/sequence_back.dsq back";
   sequence3 = "~/data/player/sequence_side.dsq side";
   sequence4 = "~/data/player/sequence_fall.dsq fall";
   sequence5 = "~/data/player/sequence_land.dsq land";
   sequence6 = "~/data/player/sequence_jump.dsq jump";

};         



// ******************************************************************
//					onAdd() 
// ******************************************************************
//
function MazeRunner::onAdd( %DB , %Obj )
{
   Parent::onAdd( %DB , %Obj );
   %Obj.lives = 3;
   livescounter.setCounterValue(%Obj.lives);
}

// ******************************************************************
//					onRemove() 
// ******************************************************************
//
function MazeRunner::onRemove( %DB , %Obj )
{
   Parent::onRemove( %DB , %Obj );
}


// ******************************************************************
//					loseALife() 
// ******************************************************************
//
// 1. Decrement the player's life counter.
// 2. End the game if we are out of lives (This function is found in ~/server/scripts/server.cs)
// 3. If the game is not over, move the avatar back to its initial spawn point, and zero its velocity.
//
function Player::loseALife( %player )
{
   // 1
   %player.lives--;
   livescounter.setCounterValue(%player.lives);
   
   // 2
   if( %player.lives <= 0 )
   {
      schedule( 0 , 0 , endGame );
      MissionGroup.schedule( 0, delete );
      return;
   }

   // 3
   %player.setVelocity("0 0 0");
   %player.setTransform(%player.spawnPointTransform);
}

