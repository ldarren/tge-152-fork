//------------------------------------------------------
// Copyright 2000-2005, GarageGames.com, Inc.
// Written, modfied, or otherwise interpreted by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Loading Mission Level Editor Scripts ---------");

function rldle() {
   exec("./LevelEditor.cs");
}

//-------------------------------------------------------------------------
//             NormalEditBlock Datablock Definitions
//-------------------------------------------------------------------------
datablock StaticShapeData( blockA )
{
   category     = "EditorBlocks";
   className    = "NormalEditBlock";
   shapeFile    = "~/data/MazeRunner/Shapes/MazeBlock/" @ $BlockSet @ "/blockA.dts"; // Advanced
   emap         = false;
   isInvincible = true;

   prevTypeDB   = "spawnBlock";
   nextTypeDB   = "fadeBlock1";

   prevDB       = "blockJ"; 
   nextDB       = "blockB";

   normalSkin   = "toon0";
   selectedSkin = "toon0_sel";
};
datablock StaticShapeData( blockB : blockA )
{
   shapeFile    = "~/data/MazeRunner/Shapes/MazeBlock/" @ $BlockSet @ "/blockB.dts"; // Advanced
   prevDB       = "blockA";
   nextDB       = "blockC";
};
datablock StaticShapeData( blockC : blockA )
{
   shapeFile    = "~/data/MazeRunner/Shapes/MazeBlock/" @ $BlockSet @ "/blockC.dts"; // Advanced
   prevDB       = "blockB";
   nextDB       = "blockD";
};
datablock StaticShapeData( blockD : blockA )
{
   shapeFile    = "~/data/MazeRunner/Shapes/MazeBlock/" @ $BlockSet @ "/blockD.dts"; // Advanced
   prevDB       = "blockC";
   nextDB       = "blockE";
};
datablock StaticShapeData( blockE : blockA )
{
   shapeFile    = "~/data/MazeRunner/Shapes/MazeBlock/" @ $BlockSet @ "/blockE.dts"; // Advanced
   prevDB       = "blockD";
   nextDB       = "blockF";
};
datablock StaticShapeData( blockF : blockA )
{
   shapeFile    = "~/data/MazeRunner/Shapes/MazeBlock/" @ $BlockSet @ "/blockF.dts"; // Advanced
   prevDB       = "blockE";
   nextDB       = "blockG";
};
datablock StaticShapeData( blockG : blockA )
{
   shapeFile    = "~/data/MazeRunner/Shapes/MazeBlock/" @ $BlockSet @ "/blockG.dts"; // Advanced
   prevDB       = "blockF";
   nextDB       = "blockH";
};
datablock StaticShapeData( blockH : blockA )
{
   shapeFile    = "~/data/MazeRunner/Shapes/MazeBlock/" @ $BlockSet @ "/blockH.dts"; // Advanced
   prevDB       = "blockG";
   nextDB       = "blockI";
};
datablock StaticShapeData( blockI : blockA )
{
   shapeFile    = "~/data/MazeRunner/Shapes/MazeBlock/" @ $BlockSet @ "/blockI.dts"; // Advanced
   prevDB       = "blockH";
   nextDB       = "blockJ";
};
datablock StaticShapeData( blockJ : blockA )
{
   shapeFile    = "~/data/MazeRunner/Shapes/MazeBlock/" @ $BlockSet @ "/blockJ.dts"; // Advanced
   prevDB       = "blockI";
   nextDB       = "blockA";
};

function StaticShapeData::dup( %DB , %Obj )
{
   %block = new StaticShape() 
   {
      position  = %Obj.getPosition();
      rotation  = "1 0 0 0";
      scale     = "1 1 1";
      dataBlock = %DB;      
   };
   
   MissionGroup.add( %block );
   
   echo("%block == " , %block);
   
   return %block;
}

//-------------------------------------------------------------------------
//             FadeBlock Datablock Definitions
//-------------------------------------------------------------------------
datablock StaticShapeData( fadeBlock1 )
{
   category     = "EditorBlocks";
   className    = "FadeEditBlock";
   shapeFile    = "~/data/MazeRunner/Shapes/MazeBlock/" @ $BlockSet @ "/blockA.dts"; // Advanced
   emap         = false;
   isInvincible = true;

   prevTypeDB   = "blockA";
   nextTypeDB   = "fireBlock0";

   prevDB       = "fadeBlock10"; 
   nextDB       = "fadeBlock2";
   
   normalSkin   = "toon0_fade1";
   selectedSkin = "toon0_fade1_sel";
};

datablock StaticShapeData( fadeBlock2 : fadeBlock1)
{
   prevDB       = "fadeBlock1"; 
   nextDB       = "fadeBlock3";

   normalSkin   = "toon0_fade2";
   selectedSkin = "toon0_fade2_sel";
};

datablock StaticShapeData( fadeBlock3 : fadeBlock1)
{
   prevDB       = "fadeBlock2"; 
   nextDB       = "fadeBlock4";

   normalSkin   = "toon0_fade3";
   selectedSkin = "toon0_fade3_sel";
};

datablock StaticShapeData( fadeBlock4 : fadeBlock1)
{
   prevDB       = "fadeBlock3"; 
   nextDB       = "fadeBlock5";

   normalSkin   = "toon0_fade4";
   selectedSkin = "toon0_fade4_sel";
};
datablock StaticShapeData( fadeBlock5 : fadeBlock1)
{
   prevDB       = "fadeBlock4"; 
   nextDB       = "fadeBlock6";

   normalSkin   = "toon0_fade5";
   selectedSkin = "toon0_fade5_sel";
};
datablock StaticShapeData( fadeBlock6 : fadeBlock1)
{
   prevDB       = "fadeBlock5"; 
   nextDB       = "fadeBlock7";

   normalSkin   = "toon0_fade6";
   selectedSkin = "toon0_fade6_sel";
};
datablock StaticShapeData( fadeBlock7 : fadeBlock1)
{
   prevDB       = "fadeBlock6"; 
   nextDB       = "fadeBlock8";

   normalSkin   = "toon0_fade7";
   selectedSkin = "toon0_fade7_sel";
};
datablock StaticShapeData( fadeBlock8 : fadeBlock1)
{
   prevDB       = "fadeBlock7"; 
   nextDB       = "fadeBlock9";

   normalSkin   = "toon0_fade8";
   selectedSkin = "toon0_fade8_sel";
};
datablock StaticShapeData( fadeBlock9 : fadeBlock1)
{
   prevDB       = "fadeBlock8"; 
   nextDB       = "fadeBlock10";

   normalSkin   = "toon0_fade9";
   selectedSkin = "toon0_fade9_sel";
};
datablock StaticShapeData( fadeBlock10 : fadeBlock1)
{
   prevDB       = "fadeBlock9"; 
   nextDB       = "fadeBlock1";

   normalSkin   = "toon0_fade10";
   selectedSkin = "toon0_fade10_sel";
};



//-------------------------------------------------------------------------
//             FireBlock Datablock Definitions
//-------------------------------------------------------------------------
datablock StaticShapeData( fireBlock0 )
{
   category     = "EditorBlocks";
   className    = "FireEditBlock";
   shapeFile    = "~/data/MazeRunner/Shapes/MazeBlock/" @ $BlockSet @ "/blockA.dts"; // Advanced
   emap         = false;
   isInvincible = true;

   prevTypeDB   = "fadeBlock1";
   nextTypeDB   = "teleportBlockRed";

   prevDB       = "fireBlock9"; 
   nextDB       = "fireBlock1";
   
   normalSkin   = "fire";
   selectedSkin = "fire_sel";
   
   type    = 0;
};

datablock StaticShapeData( fireBlock1  : fireBlock0 )
{
   prevDB       = "fireBlock0"; 
   nextDB       = "fireBlock2";
   type    = 1;
};
datablock StaticShapeData( fireBlock2  : fireBlock0 )
{
   prevDB       = "fireBlock1"; 
   nextDB       = "fireBlock3";
   type    = 2;
};
datablock StaticShapeData( fireBlock3  : fireBlock0 )
{
   prevDB       = "fireBlock2"; 
   nextDB       = "fireBlock4";
   type    = 3;
};
datablock StaticShapeData( fireBlock4  : fireBlock0 )
{
   prevDB       = "fireBlock3"; 
   nextDB       = "fireBlock5";
   type    = 4;
};
datablock StaticShapeData( fireBlock5  : fireBlock0 )
{
   prevDB       = "fireBlock4"; 
   nextDB       = "fireBlock6";
   type    = 5;
};
datablock StaticShapeData( fireBlock6  : fireBlock0 )
{
   prevDB       = "fireBlock5"; 
   nextDB       = "fireBlock7";
   type    = 6;
};
datablock StaticShapeData( fireBlock7  : fireBlock0 )
{
   prevDB       = "fireBlock6"; 
   nextDB       = "fireBlock8";
   type    = 7;
};
datablock StaticShapeData( fireBlock8  : fireBlock0 )
{
   prevDB       = "fireBlock7"; 
   nextDB       = "fireBlock9";
   type    = 8;
};

datablock StaticShapeData( fireBlock9  : fireBlock0 )
{
   prevDB       = "fireBlock8"; 
   nextDB       = "fireBlock0";

   normalSkin   = "fire_random";
   selectedSkin = "fire_random_sel";
   
   type    = 9;
};


function FireEditBlock::onNewDatablock( %DB , %Obj )
{
   // Change marker type (firing direction)
   %Obj.type = %DB.type;
   
   if( isObject( %Obj.bullet ) ) %Obj.bullet.delete();
   
   %Obj.doFire();
}



//-------------------------------------------------------------------------
//             Teleport Station Datablock Definitions
//-------------------------------------------------------------------------
datablock StaticShapeData( teleportBlockRed )
{
   category     = "EditorBlocks";
   className    = "TeleportEditBlock";
   shapeFile    = "~/data/MazeRunner/Shapes/MazeBlock/" @ $BlockSet @ "/blockA.dts"; // Advanced
   emap         = false;
   isInvincible = true;

   prevTypeDB   = "fireBlock0";
   nextTypeDB   = "coinBlock";

   prevDB       = "teleportBlockBlue"; 
   nextDB       = "teleportBlockGreen";
   
   normalSkin   = "toon0_red_tele";
   selectedSkin = "toon0_red_tele_sel";
};

datablock StaticShapeData( teleportBlockGreen : teleportBlockRed )
{
   prevDB       = "teleportBlockRed"; 
   nextDB       = "teleportBlockBlue";

   normalSkin   = "toon0_green_tele";
   selectedSkin = "toon0_green_tele_sel";
};

datablock StaticShapeData( teleportBlockBlue : teleportBlockRed )
{
   prevDB       = "teleportBlockGreen"; 
   nextDB       = "teleportBlockRed";

   normalSkin   = "toon0_blue_tele";
   selectedSkin = "toon0_blue_tele_sel";
};


//-------------------------------------------------------------------------
//             Coin Marker Datablock Definitions
//-------------------------------------------------------------------------
datablock StaticShapeData( coinBlock )
{
   category     = "EditorBlocks";
   className    = "CoinEditBlock";
   shapeFile    = "~/data/MazeRunner/Shapes/MazeBlock/" @ $BlockSet @ "/blockA.dts"; // Advanced
   emap         = false;
   isInvincible = true;

   prevTypeDB   = "teleportBlockRed";
   nextTypeDB   = "spawnBlock";

   prevDB       = "coinBlock"; 
   nextDB       = "coinBlock";
   
   normalSkin   = "coin";
   selectedSkin = "coin_sel";
};






//-------------------------------------------------------------------------
//             Spawn Marker Datablock Definitions
//-------------------------------------------------------------------------
datablock StaticShapeData( spawnBlock )
{
   category     = "EditorBlocks";
   className    = "SpawnEditBlock";
   shapeFile    = "~/data/MazeRunner/Shapes/MazeBlock/" @ $BlockSet @ "/blockA.dts"; // Advanced
   emap         = false;
   isInvincible = true;

   prevTypeDB   = "coinBlock";
   nextTypeDB   = "blockA";

   prevDB       = "spawnBlock"; 
   nextDB       = "spawnBlock";
   
   normalSkin   = "spawn";
   selectedSkin = "spawn_sel";
};






// ******************************************************************
//					BuildLevel() 
// ******************************************************************
//
// 1. Reset the working elevation.
// 2. Create a new file object and attempt to open a level file.
// 3. Read the 'next mission' line from the level file.
// 4. Create all the SimGroups we will use to organize the level contents.
// 5. Parse the file until we hit the end-of-file
// 6. Parse the layer 'task'.
// 6a. Adjust Elevation Up
// 6b. Adjust Elevation Down
// 6c. Prepare to lay down a layer
// 6d. Bad layer task defintion
// 7. Parse the layer 'type'.
// 7a. Put blocks in level
// 7b. Put obstacles in level
// 7c. Put pickups in level
// 7d. Drop the player into the game
// 7e. Bad layer type defintion
// 8. Close the file and delete the file object.
// 9. Start any fadeblocks and fireball blocks we have
// 10. Set the initial coin counter
//
function EditLevel( %levelNum ) 
{
   $Camera::movementSpeed = 100;
   // 1
   $CurrentElevation = $BaseElevation;



   if( isObject( levelEditGroup )  ) levelEditGroup.delete(); // Advanced

   MissionGroup.add( new SimGroup( levelEditGroup ) ); // Advanced
   
   levelEditGroup.add( new SimGroup( currentBlocks ) ); // Advanced

   


   // 2
   $Game::Camera.setTransform("-19.2685 -4.94996 115.346 0.397309 -0.343442 0.850995 1.58645");
   $Game::Client.setControlObject( $Game::Camera );

   %block = new StaticShape() 
   {
      position  = "0 0" SPC $BaseElevation;
      rotation  = "1 0 0 0";
      scale     = "1 1 1";
      dataBlock = "blockA";      
   };
   
   currentBlocks.add( %block );
   
   return;

   // 3
   
   if( isObject( gameLevelGroup )  )
      gameLevelGroup.delete();

   MissionGroup.add( new SimGroup( gameLevelGroup ) );

   gameLevelGroup.add( new SimGroup( mazeBlocksGroup ) );
   gameLevelGroup.add( new SimGroup( fadeGroup ) );   
   gameLevelGroup.add( new SimGroup( FireBallMarkersGroup ) );   
   gameLevelGroup.add( new SimGroup( TeleportStationGroupX ) );   
   gameLevelGroup.add( new SimGroup( TeleportStationGroupY ) );   
   gameLevelGroup.add( new SimGroup( TeleportStationGroupZ ) );   
   gameLevelGroup.add( new SimGroup( TeleportStationEffectsGroup ) );   
   gameLevelGroup.add( new SimGroup( CoinsGroup ) );   
   
   // 4
   coincounter.setCounterValue( CoinsGroup.getCount() );
}

// ******************************************************************
//					layOutBlocks() 
// ******************************************************************
//
// 1. Iterate over the arrayObject, line by line, token by token.
// 2. Select class of object to create.
// 2a. Create a normal block.
// 2a. Creaet a fade block.
// 3. Destroy the array object.
//
function layOutBlocks( %layerArray )
{
   // 1
   for(%x = 0; %x < 16; %x++) 
   {
      %row = 16;

      for(%y = 0; %y < 16; %y++)       
      {
         %row--;
         %actX = (%x * 4) - 30;
         %actY = (%y * 4) - 30;

         %layer = %layerArray.getEntry( %row );

         %blockType = getSubStr( %layer , %x , 1 );

         // 2
         switch$($BLOCKCLASS[%blockType])
         {
            // 2a
            case NORMAL:
               %block =  new TSStatic() 
               {
                  shapeName	= "~/data/MazeRunner/Shapes/MazeBlock/" @ $BlockSet @ "/block" @ %blockType @ ".dts"; // Advanced
                  position	   = %actX SPC %actY SPC $CurrentElevation;
                  scale		   = "1 1 1";
               };
               mazeBlocksGroup.add(%block);

            // 2b
            case FADE:
               %block = new StaticShape() {
                  position  = %actX SPC %actY SPC $CurrentElevation;
                  rotation  = "1 0 0 0";
                  scale     = "1 1 1";
                  dataBlock = "FadeBlock";
                  timer     = (%blockType + 1) * 1000;
                  action    = "waitToFadeOut";
                  maxTime   = (%blockType + 1) * 1000;
               };     
               fadeGroup.add(%block);
         }
      }
   }

   // 3
   %layerArray.delete();
}


// ******************************************************************
//					layOutObstacles() 
// ******************************************************************
//
// 1. Iterate over the arrayObject, line by line, token by token.
// 2. Select class of object to create.
// 2a. Create a fireballl block.
// 2a. Creaet a teleport station.
// 3. Destroy the array object.
//
//
function layOutObstacles( %layerArray )
{
   // 1
   for(%x = 0; %x < 16; %x++) 
   {
      %row = 16;

      for(%y = 0; %y < 16; %y++)       
      {
         %row--;
         %actX = (%x * 4) - 30;
         %actY = (%y * 4) - 30;

         %layer = %layerArray.getEntry( %row );

         %obstacleType = getSubStr( %layer , %x , 1 );

         // 2
         switch$($OBSTACLECLASS[%obstacleType])
         {
            // 2a
            case FIREBALL:
               //echo("\c2 Loading fireball");
               %fireballMarker = new StaticShape() {
                  position  = %actX SPC %actY SPC $CurrentElevation;
                  rotation  = "1 0 0 0";
                  scale     = "1 1 1";
                  dataBlock = "FireBallBlock";
                  type      = %obstacleType;
               };     
               %fireballMarker.setSkinName( fire );

               FireBallMarkersGroup.add(%fireballMarker);

            // 2b
            case TELEPORT:
               echo("\c2 Loading teleport station");
               %teleportStation = new Trigger() {
                  position  = ( %actX - 2 ) SPC ( %actY + 2 ) SPC $CurrentElevation;
                  rotation  = "1 0 0 0";
                  scale     = "4 4 4";
                  polyhedron = "0.0000000 0.0000000 0.0000000 1.0000000 0.0000000 0.0000000 0.0000000 -1.0000000 0.0000000 0.0000000 0.0000000 1.0000000";
                  dataBlock = "TeleportTrigger";
                  type      = %obstacleType;
                  group     = (TeleportStationGroup @ %obstacleType).getID();
                  enabled   = true;
               };     
               %teleportStation.attachEffect();

               (TeleportStationGroup @ %obstacleType).add(%teleportStation);

               TeleportStationEffectsGroup.add( %teleportStation.myEffect );
               TeleportStationEffectsGroup.add( %teleportStation.myPZone );
               
         }
      }
   }
   
   // 3
   %layerArray.delete();
}

// ******************************************************************
//					layOutPickups() 
// ******************************************************************
//
// 1. Iterate over the arrayObject, line by line, token by token.
// 2. Select class of object to create.
// 2a. Create a coin.
// 3. Destroy the array object.
//
function layOutPickups( %layerArray )
{
   // 1
   for(%x = 0; %x < 16; %x++) 
   {
      %row = 16;

      for(%y = 0; %y < 16; %y++)       
      {
         %row--;
         %actX = (%x * 4) - 30;
         %actY = (%y * 4) - 30;

         %layer = %layerArray.getEntry( %row );

         %blockType = getSubStr( %layer , %x , 1 );

         // 2
         switch$($PICKUPCLASS[%blockType])
         {
            //2a
            case COIN:
               %coin =  new item() 
               {
                  position	   = %actX SPC %actY SPC $CurrentElevation;
                  dataBlock	= Coin;
                  static		= 1;
                  scale		   = "1 1 1";
               };

               CoinsGroup.add(%coin);
               //%coin.applyImpulse(%coin.getWorldBoxCenter(), "0 0 1");
         }
      }
   }
   
   //3
   %layerArray.delete();
}

// ******************************************************************
//					playerDrop() 
// ******************************************************************
//
// 1. Iterate over the arrayObject, line by line, token by token.
// 2. Select class of object to create.
// 2a. Drop the player in (after a short pause).
// 3. Destroy the array object.
//
function playerDrop( %layerArray )
{
   // 1
   for(%x = 0; %x < 16; %x++) 
   {
      %row = 16;

      for(%y = 0; %y < 16; %y++)       
      {
         %row--;
         %actX = (%x * 4) - 30;
         %actY = (%y * 4) - 30;

         %layer = %layerArray.getEntry( %row );

         %blockType = getSubStr( %layer , %x , 1 );

         // 2
         switch$($DROPCLASS[%blockType])
         {
            // 2a
            case DROP:
               $Game::Player.setTransform ( pickSpawnPoint() );
               
               $Game::Player.spawnPointTransform = (%actX SPC %actY SPC $CurrentElevation);
               $Game::Player.schedule( 2800 , setVelocity , "0 0 0" );
               $Game::Player.schedule( 3000 , setTransform , %actX SPC %actY SPC $CurrentElevation );
               %layerArray.delete();
               return;

         }
      }
   }
   // 3
   %layerArray.delete();
}

