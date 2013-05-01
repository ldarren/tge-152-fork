//------------------------------------------------------
// Copyright 2000-2005, GarageGames.com, Inc.
// Written, modfied, or otherwise interpreted by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

echo("\n\c3--------- Loading Mission Level Loader Scripts ---------");
function rldll() {
   exec("./LevelLoader.cs");
}
$CurrentLevelMap  = 0;
$BaseElevation    = 90; // Starting placement elevation (do not modify in scripts)
$LevelIncrement   = 4;  // Elevation Increment (do not modify in scripts)

$MinElevation     = 70; 
$MaxOffCenter     = $LevelIncrement * 64 ; 

$CurrentElevation = $BaseElevation ; // Working elevation ( we modify this variable ).


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
function BuildLevel( %levelNum ) {

   // 1
   $CurrentElevation = $BaseElevation ;

   // 2
   %file = new FileObject();
   %fileName = expandFileName( "~/LevelMaps/Level" @ %levelNum @ ".txt" );

   %fileIsOpen = %file.openForRead( %fileName );
   if( ! %fileIsOpen ) 
   {
      %levelNum = 0;
      $Game::NextLevelMap =0;
      %fileName = expandFileName( "~/LevelMaps/Level" @ %levelNum @ ".txt" );
   }
   
   %fileIsOpen = %file.openForRead( %fileName );
   if( ! %fileIsOpen ) 
   {
      return false;
   }

   // 3
   $Game::NextLevelMap++;

   // 4
   if( isObject( gameLevelGroup )  )
      gameLevelGroup.delete();

   MissionGroup.add( new SimGroup( gameLevelGroup ) );

   gameLevelGroup.add( new SimGroup( mazeBlocksGroup ) );
   gameLevelGroup.add( new SimGroup( fadeGroup ) );   
   gameLevelGroup.add( new SimGroup( FireBallMarkersGroup ) );   
   gameLevelGroup.add( new SimGroup( TeleportStationGroupRed ) );   
   gameLevelGroup.add( new SimGroup( TeleportStationGroupGreen ) );   
   gameLevelGroup.add( new SimGroup( TeleportStationGroupBlue ) );   
   gameLevelGroup.add( new SimGroup( TeleportStationEffectsGroup ) );   
   gameLevelGroup.add( new SimGroup( CoinsGroup ) );   
   gameLevelGroup.add( new SimGroup( RandomSpawnGroup ) );   

   // 5
   while(!%file.isEOF() ) 
   {
      %layer = %file.readLine();

      %blockType     = strlwr( getWord( %layer , 0 ) );
      %blockPosition = getWords( %layer , 1 , 3 );
      
      echo(%layer);

      if( strstr( %blockType, "spawn" ) != -1 )
      {
         echo("\c3 Spawn");
         %tmpSpawnPoint = new ScriptObject();
         RandomSpawnGroup.add(%tmpSpawnPoint);
         %tmpSpawnPoint.position = %blockPosition;
      }
      else if( strstr( %blockType, "teleport" ) != -1 )
      {        
         %type = strReplace( %blockType , "teleportblock" , "" );
         
         echo("\c3 Teleport - ", %type);
         
         %teleportStation = new Trigger() {
            position  = vectorAdd( %blockPosition , "-0.5 0.5 0" );
            rotation  = "1 0 0 0";
            scale     = "1 1 1";
            polyhedron = "0.0000000 0.0000000 0.0000000 1.0000000 0.0000000 0.0000000 0.0000000 -1.0000000 0.0000000 0.0000000 0.0000000 1.0000000";
            dataBlock = "TeleportTrigger";
            type      = %type;
            group     = ("TeleportStationGroup" @ %type).getID();
            enabled   = true;
         };     
         %teleportStation.attachEffect();
         
         ("TeleportStationGroup" @ %type).add( %teleportStation );
      }
      else if( strstr( %blockType, "fire" ) != -1 )
      {
         %blockDB = "FireBallBlock";
         
         %type = strReplace( %blockType , "fireblock" , "" );
         
         switch(%type)
         {
         case 0: %orientation = "0 0 1 180";
         case 1: %orientation = "0 0 1 225";
         case 2: %orientation = "0 0 1 270";
         case 3: %orientation = "0 0 -1 45";
         case 4: %orientation = "0 0 1 0";
         case 5: %orientation = "0 0 1 45";
         case 6: %orientation = "0 0 1 90";
         case 7: %orientation = "0 0 1 135";
         case 8: %orientation = "-1 0 0 90";
         default: %orientation = "1 0 0 0";
         }
         
         echo("\c3 Fire Block");
         //echo("\c2 Loading fireball");
         
         if( 8 == %type )
         {
            %fireballMarker = new StaticShape() 
            {
               position  = vectorAdd("0 2 0" , %blockPosition );
               rotation  = %orientation;
               scale     = "1 1 1";            
               dataBlock = %blockDB;
               type      = %type;
               orientation = %orientation;
            }; 
         }
         else
         {
            %fireballMarker = new StaticShape() 
            {
               position  = %blockPosition;
               rotation  = %orientation;
               scale     = "1 1 1";            
               dataBlock = %blockDB;
               type      = %type;
            }; 
         }
         
         
         if( %blockDB $= "FireBallBlock") 
         {
         %fireballMarker.setSkinName( fire );
         }

         FireBallMarkersGroup.add(%fireballMarker);

      }
      else if( strstr( %blockType, "coin" ) != -1 )
      {
         echo("\c3 Coin");
         %coin =  new item() 
         {
            position	   = %blockPosition;
            dataBlock	   = Coin;
            static		    = 1;
            scale		     = "1 1 1";
         };
      }
      else if( strstr( %blockType, "fade" ) != -1 )      
      {
         echo("\c3 FadeBlock");
         %fadeTime = strReplace( %blockType , "fadeblock" , "" );
         %block = new StaticShape() {
            position  = %blockPosition;
            rotation  = "1 0 0 0";
            scale     = "1 1 1";
            dataBlock = "FadeBlock";
            timer     = %fadeTime * 1000;
            action    = "waitToFadeOut";
            maxTime   = %fadeTime * 1000;
         }; 
         fadeGroup.add(%block);    
      }
      else // Must be a normal block
      {
         echo("\c3 NormalBlock");
         
         switch$( %blockType )
         {
         case "blockH":
            %block =  new TSStatic() 
            {
               shapeName	 = "~/data/blocks/blockE.dts"; // Advanced
               position	  = %blockPosition;
               scale		   = "1 1 1";
               rotation  = "0 0 1 90";
               };
         case "blockI":
            %block =  new TSStatic() 
            {
               shapeName	 = "~/data/blocks/blockF.dts"; // Advanced
               position	  = %blockPosition;
               scale		   = "1 1 1";
               rotation  = "0 0 1 90";
               };
         case "blockJ":
            %block =  new TSStatic() 
            {
               shapeName	 = "~/data/blocks/blockG.dts"; // Advanced
               position	  = %blockPosition;
               scale		   = "1 1 1";
               rotation  = "0 0 1 90";
               };
         default:
            %block =  new TSStatic() 
            {
               shapeName	 = "~/data/blocks/"  @ %blockType @ ".dts"; // Advanced
               position	 = %blockPosition;
               scale		   = "1 1 1";
               };
         }
         mazeBlocksGroup.add(%block);
      }
   }

   // 8 
   %file.close();
   %file.delete();

   // 9
   if( fadeGroup.getCount() )
      fadeGroup.schedule( 5000 , fadePass );

   if( FireBallMarkersGroup.getCount() )
      FireBallMarkersGroup.schedule( 1500 , firePass );   

   // 10
   echo("\c3 STEP 10 ");
   coincounter.setCounterValue( CoinsGroup.getCount() );
   
   // 11
   $Game::Player.setTransform ( "0 0 10000" );

   %spawnPos = RandomSpawnGroup.getRandomObject().position; 
   
   $Game::Player.spawnPointTransform = %spawnPos;

   $Game::Player.schedule( 900 , setVelocity , "0 0 0" );
   $Game::Player.schedule( 1000 , setTransform , %spawnPos );
   $Game::Player.schedule( 1100 , applyImpulse , "0 0 0" , "0 0 70" );
   
   serverConnection.setBlackout(1, 0 );
   
   serverConnection.schedule( 1200, setBlackout , 0 , 1500 );
   
}

