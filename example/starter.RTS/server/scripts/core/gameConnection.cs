//-----------------------------------------------------------------------------
// GameConnection Methods
// These methods are extensions to the GameConnection class. Extending
// GameConnection make is easier to deal with some of this functionality,
// but these could also be implemented as stand-alone functions.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

function RTSConnection::getClientIndex(%this)
{
   // Find our index in the client group...
   for(%i=0; %i<ClientGroup.getCount(); %i++)
      if(ClientGroup.getObject(%i) == %this)
         return %i;
         
   return -1;
}

function RTSConnection::onClientEnterGame(%this)
{
   commandToClient(%this, 'SyncClock', $Sim::Time - $Game::StartTime);
  // echo("Server--RTSConnection::onClientEnterGame, %this is " @ %this );
   %unitsPerDir = 4;

   // Add to team
   %clientIndex = %this.getClientIndex();
   %this.setTeam(%clientIndex);

   %offset = ((%this.getTeam()-1) * (%unitsPerDir + 5));
   
   // Create simset to track all units
   %this.units = new SimGroup();
 
   %this.createPlayer((%offset*5) SPC (%offset*5) SPC "250", 3);
   %this.createPlayer((%offset*5 + 5) SPC (%offset*5 + 5) SPC "250", 3);
   %this.createPlayer((%offset*5+10) SPC (%offset*5) SPC "250", 3);
   %this.createPlayer((%offset*5 + 0) SPC (%offset*5 + 5) SPC "250", 3);

  
   // Create simset to track selection
   %this.selection = new SimSet();
   
   // Create simset to track buildings
   %this.buildings = new SimGroup();

   // Create an observer cam for fly mode.
   // Create a new camera object.
   %this.observerCam = new Camera() {
      dataBlock = Observer;
   };
   MissionCleanup.add( %this.observerCam );
   
   // Create a new RTS camera object.
   %this.rtsCam = new RTSCamera()
   {
      dataBlock = RTSObserver;
   };
   MissionCleanup.add( %this.rtsCam );

   // Do scoping
   %this.observerCam.scopeToClient(%this);
   %this.rtsCam.scopeToClient(%this);
   // Scope all the static resources.
   // sure we need to do this?
   if(isObject(ResourceSet))
     {
        for(%d = 0; %d < ResourceSet.getCount(); %d++)
        {
          %tr = ResourceSet.getObject(%d);
//          echo("Scoping resource (" @ %tr @ ")");
          %tr.scopeToClient(%this);
        }

   }
   // Start out with the rts cam.
   %this.setControlObject(%this.rtsCam);

//   echo("RTSConnection::onClientEnterGame--setting up resource store");   
   %this.resourceStore = resourceStore::Ctor();
   commandToClient(%this, 'AcceptSetupStores', "LOCAL"); 
}

function RTSConnection::onClientLeaveGame(%this)
{
   if (isObject(%this.observerCam))
      %this.observerCam.delete();
   if (isObject(%this.rtsCam))
      %this.rtsCam.delete();
   if (isObject(%this.selection))
      %this.selection.delete();
      
   // Delete all the player buildings and units? Sure why not.
   // You could also turn them over to allies or something here
   for( %i = 0; %i < %this.units.getCount(); %i++ )
   {
      %unit = %this.units.getObject(%i);

      // Kind of hacky here
      RTSUnitData::onDisabled( 0, %unit, 0 );
      // SRZ: this looks like a mem leak--shouldn't we delete it?
      echo( "Client has disconnected, removing unit " @ %unit );
      // Let them die naturally - BJG  
      //%unit.delete();
   }


   for ( %i = 0; %i < %this.buildings.getCount(); %i++ )
   {
   	%unit = %this.buildings.getObject(%i);
   	RTSUnitData::onDisabled( 0, %unit, 0 );
   }

// SRZ: Tier2 Imp needed: We want to come up with a mechanism to handle buildings/units
// with no controlling connection, so we can leave them around and active without a controlling %client

   %this.buildings.delete();
      
   if ( isObject( %this.resourceStore ) )
   {
   	  for ( %i = 0; %i < %this.resourceStore.getCount(); %i++ )
   	  {
   	  	%resource = %this.resourceStore.getObject(%i);
   	  	%resource.delete();
   	  }
   
   	%this.resourceStore.delete();
   }
}

//-----------------------------------------------------------------------------

function RTSConnection::createPlayer(%this, %spawnPoint, %index)
{
   echo ("RTSConnection::createPlayer()--this (client?) is (" @ %this @ 
          ") unit type is (" @ %index @ ")");
   switch(%index)
   {
      case 0:
         %data = botBlock;
      case 1:
//-----------------------Begin Bug Fix: http://garagegames.com/mg/forums/result.thread.php?qt=23338
         %data = shockerBlock;
      case 2:
         %data = riflemanBlock;
//-----------------------End Bug Fix 
      case 3:
         %data = villagerBlock;
   }
      
   %player = new RTSUnit() 
   {
      scale = "1 1 1";
      dataBlock = %data;
//      shapeFile = "~/game/data/players/player/player.dts";
      path = "";
   };
   echo("RTSConnection::createPlayer--Created new %player" SPC %player SPC "for Connection " @ %this SPC
     "dataBlock =" SPC %player.getDataBlock().getName() );

//   MissionCleanup.add(%player);
   
   %player.setControllingConnection(%this);
   %player.setSkinName( getWord($Pref::Server::TeamInfo[%this.getTeam()], 3) );
   %player.setTeam(%this.getTeam());
   %player.setTransform(%spawnPoint);
   %player.client = %this;
   %player.maxAmt = 25;                    
   %player.stats["Kills"] = (getRandom() * 5) % 5;
   %player.stats["Damage Delt"] = (getRandom() * 1000) % 1000;
   
   // Add the unit to the group of units
   %this.units.add(%player);
}

//-----------------------------------------------------------------------------

function RTSConnection::onLeaveMissionArea(%this)
{
   // The control objects invoked this method when they
   // move out of the mission area.
}

function RTSConnection::onEnterMissionArea(%this)
{
   // The control objects invoked this method when they
   // move back into the mission area.
}
