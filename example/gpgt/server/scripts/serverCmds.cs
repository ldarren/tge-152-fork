//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

function serverCmdGetPlayerGhostIndex( %clientConn )
{
   echo("\c3 serverCmdGetPlayerGhostIndex");

   if( !isObject( %clientConn.player ) )
      return;

   %playerGhostIndex = %clientConn.getGhostID( %clientConn.player );

   echo("\c4 tell client setGhostID " , %playerGhostIndex);

   commandToClient( %clientConn, 'setGhostID' , %playerGhostIndex );
}


function serverCmdDropABot( %clientConn, %botCommand )
{
   if ( arenaPlayer::isHacked(%clientConn.player) ) return;

   if( !$Game::Running ) return;

   %thePlayer = %clientConn.player;
   %curEnergy = %thePlayer.getEnergyLevel();

   if( ( getSimTime() - %thePlayer.lastCommandTime ) < $arena::minCommandDelay )   
   {
      return;
   }

   if ( playGui.botCount >= $arena::MaxBots) return;

   switch$( %botCommand )
   {
   case "sendHackBot":   
      if ( ( %curEnergy >= $arena::HackBotEnergyCost ) &&
           ( playGui.hackBotCount < $arena::MaxHackBots ) )
      {
         %theBot = arenaPlayer::dropABot( %thePlayer , hackBot );

         %thePlayer.setEnergyLevel( %curEnergy - $arena::HackBotEnergyCost );
         %thePlayer.lastCommandTime = getSimTime();
         %theBot.currentTarget = %thePlayer.currentTarget;
      }
      

   case "sendAttackBot": 
      if( ( %curEnergy >= $arena::AttackBotEnergyCost )  &&
          ( playGui.attackBotCount < $arena::MaxAttackBots ) )
      {
         %thePlayer.camPointingVector = %thePlayer.client.camera.getForwardVector();
         echo("\c5 %thePlayer.camPointingVector == ", %thePlayer.camPointingVector );

         %theBot = arenaPlayer::dropABot( %thePlayer , attackBot );

         %thePlayer.setEnergyLevel( %curEnergy - $arena::AttackBotEnergyCost );
         %thePlayer.lastCommandTime = getSimTime();         
      }

   case "sendEnergyBot": 
      if( ( %curEnergy >= $arena::EnergyBotEnergyCost )  &&
          ( playGui.energyBotCount < $arena::MaxEnergyBots ) )
      {
         %theBot = arenaPlayer::dropABot( %thePlayer , energyBot );

         %thePlayer.setEnergyLevel( %curEnergy - $arena::MaxEnergyBots );
         %thePlayer.lastCommandTime = getSimTime();
      }
   }
}




//-----------------------------------------------------------------------------
// Inventory server commands
//-----------------------------------------------------------------------------
function serverCmdUse( %clientConn , %useDBName )
{
	echo("serverCmdUse( "@%clientConn@" , "@%useDBName@" )");
	%Obj = %clientConn.getControlObject();
	%DB  = %Obj.getDataBlock();

	// 
	// To be sure and explicitly covert DB to an ID
	//
	%useDB = %useDBName.getID();

	//echo("calling...doUse( "@%Obj@" , "@%useDB@" )");

	%DB.doUse( %Obj , %useDB );
}


function serverCmdThrow( %clientConn , %throwDBName )
{
	echo("serverCmdThrow( "@%clientConn@" , "@%throwDBName@" )");
	%Obj = %clientConn.getControlObject();
	%DB  = %Obj.getDataBlock();

	// 
	// To be sure and explicitly covert DB to an ID
	//

	%throwDB = %throwDBName.getID();

	//echo("calling...doThrow( "@%Obj@" , "@%throwDB@" )");

	%DB.doThrow( %Obj , %throwDB );
}

//-----------------------------------------------------------------------------
// Misc. server commands avialable to clients
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

function serverCmdToggleCamera(%clientConn)
{
   %control = %clientConn.getControlObject();
   if (%control == %clientConn.player)
   {
      %control = %clientConn.camera;
      %control.mode = toggleCameraFly;
   }
   else
   {
      %control = %clientConn.player;
      %control.mode = observerFly;
   }
   %clientConn.setControlObject(%control);
}

function serverCmdDropPlayerAtCamera(%clientConn)
{
   if ( isObject(EditorGui) )
   {
      %clientConn.player.setTransform(%clientConn.camera.getTransform());
      %clientConn.player.setVelocity("0 0 0");
      %clientConn.setControlObject(%clientConn.player);
   }
}

function serverCmdDropCameraAtPlayer(%clientConn)
{
   %clientConn.camera.setTransform(%clientConn.player.getEyeTransform());
   %clientConn.camera.setVelocity("0 0 0");
   %clientConn.setControlObject(%clientConn.camera);
}


//-----------------------------------------------------------------------------
function serverCmdSuicide(%clientConn)
{
   if (isObject(%clientConn.player))
   {
      %clientConn.player.kill("Suicide");
   }
}   

function serverCmdPlayCel(%clientConn,%anim)
{
   if (isObject(%clientConn.player))
   {
      %clientConn.player.playCelAnimation(%anim);
   }
}

function serverCmdPlayDeath(%clientConn)
{
   if (isObject(%clientConn.player))
   {
      %clientConn.player.playDeathAnimation();
   }
}


