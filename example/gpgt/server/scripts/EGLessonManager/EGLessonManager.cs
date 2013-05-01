//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
function rldlm()
{
   exec("./EGLessonManager.cs");
}
function loadGPGTLessons() 
{

   echo("\n\c3******************** loadGPGTLessons() - finding and loading lessons");


   //// Find every map file and load lesson file specified within
   //
   %mapFile = findFirstFile("*/lesson.map.txt");

   while("" !$= %mapFile) 
   {

      %file = new FileObject();

      if( %file.openForRead( %mapFile ) ) 
      {
         %lessonFile    = %file.readLine();

         // *****************************************************************************
         // Note: This was placed here to allow the author a quick way to turn off lesson
         //       maps while preparing the guides.  It has been left here for your use, 
         //       but the best way to remove a lesson from a kit is to move the lesson
         //       directory to some offline storage location.
         // *****************************************************************************
         if("skipme" !$= %lessonFile) 
         {
            %lessonFile    = filePath( %mapFile ) @ "/" @ %lessonFile;

            %lessonName    = %file.readLine();

            exec( %lessonFile );

            echo("\c3 ^^^^^^^ Loaded lesson=> ", %lessonName, "\n"); 
         }
      } 
      else 
      {
         error("loadGPGTLessons() - Failed to open lesson map:", %mapFile );
      }

      %file.delete();

      %mapFile = findNextFile("*/lesson.map.txt");
   }

}


function serverCmdAllClientsLoadActionMap( %client , %path ) {
   echo("\c3serverCmdAllClientsLoadActionMap( ", %client , " , " , %path , " )" );

   if( %client.isAdmin )
   {      
      $currentLessonPath = %path;
      echo("\c5 setting currentlesson path: ", $currentLessonPath);
   }

   %count = ClientGroup.getCount();
   for(%cl= 0; %cl < %count; %cl++)
   {
      %client = ClientGroup.getObject(%cl);
      commandToClient( %client , 'loadActionMap', %path );
   }
}


function serverCmdLoadLesson( %client , %lessonName ) {
   $currentLessonPath = ""; 
   echo("\c3serverCmdLoadLesson( ", %client , " , " , %lessonName , " )" );

   //// 0 - Only allow client that is admin start lessons.
   //
   // Note: This prevents students from changing the current
   // lesson during an interactive lecture.
   //   
   if( !%client.isAdmin) return;

   //// 1 - Verify that MissionGroup is present, or we can't proceed
   //   
   if(!isObject(MissionGroup)) 
   {
      error("ERROR::serverCmdLoadLesson() No MissionGroup!");
      return;
   }

   //// 2 - Delete last LessonGroup (Starts automatic cleanup)
   //
   if(isObject(LessonGroup)) 
   {
      LessonGroup.delete();
   }

   //// 3 - Create a fresh 'LessonGroup' simgroup in MissionGroup
   //
   // Note: This group will contain all lesson objects AND the lesson
   //       ScriptObject.
   //
   echo("\c3serverCmdLoadLesson():: Creating LessonGroup ");
   MissionGroup.add( new SimGroup( LessonGroup ) );

   //// 4 - Create the lesson.  
   //
   // Note: By creating a script object with a name 'myLesson',
   //       we cause myLesson::onAdd() to be called.
   //       This call to onAdd(), tells the lesson to prepare
   //       itself.
   //
   //       Every lesson defines an onAdd() and onRemove(), among
   //       other callbacks and methods.  
   //
   echo("\c3serverCmdLoadLesson():: Starting lesson: ", %lessonName);

   LessonGroup.CurrentLessonObject = new ScriptGroup(%lessonName);

   if(!isObject( LessonGroup.CurrentLessonObject ) )
   {
      error("ERROR::serverCmdLoadLesson() Failed to create lesson ScriptGroup!");
      return;
   }

   LessonGroup.add(LessonGroup.CurrentLessonObject);

   //// 5 - Start the lesson running...
   //
   echo("\c3serverCmdLoadLesson():: Executing lesson...");
   LessonGroup.CurrentLessonObject.ExecuteLesson();
}

$GPGT::NOMARKERS = 0x1;
$GPGT::NOSIGNS   = 0x2;


function DefaultLessonPrep( %flags ) {
   echo("Executing DefaultLessonPrep");

   // Create a set of known ObjectMarkers

   LessonGroup.add(new SimGroup(ObjectMarkers));

   %direction[0] = "North";
   %direction[1] = "East";
   %direction[2] = "South";
   %direction[3] = "West";
   %directionVector[0] = "0 1 0";
   %directionVector[1] = "1 0 0";
   %directionVector[2] = "0 -1 0";
   %directionVector[3] = "-1 0 0";


   for(%meters = 10; %meters <= 70; %meters += 10) 
   {
      for(%direction = 0; %direction < 4; %direction++) 
      {
         %newPosition = vectorScale(%directionVector[%direction], %meters);
         %objectMarker = 
            new SpawnSphere(%direction[%direction] @ %meters) 
         {
            position = vectorAdd(CentralDropPoint.getPosition(), %newPosition);
            dataBlock = "SpawnSphereMarker";
            scale = "1 1 1";
            rotation = "1 0 0 0";
            radius = 1;
         };

         DropObject(%objectMarker, "0 0 25");
         ObjectMarkers.add(%objectMarker);

         if( !(%flags & $GPGT::NOMARKERS) )
         {

            %LocationBBoard = 
               new TSStatic(%direction[%direction] @ %meters @ "Billboard") 
            {
               position = vectorAdd(CentralDropPoint.getPosition(), %newPosition);
               shapeName = "~/data/lessonSupport/DropPositionBillboards/Drop" @ %meters @ ".dts";
            };

            switch(%direction) {
            case 0:	DropObject(%LocationBBoard, "3 0 1");
            case 1:	DropObject(%LocationBBoard, "0 -3 1");
            case 2:	DropObject(%LocationBBoard, "-3 0 1");
            case 3:	DropObject(%LocationBBoard, "0 3 1");
            }

            ObjectMarkers.add(%LocationBBoard);
         }

      }
   }
 if( !(%flags & $GPGT::NOSIGNS) )
 {
	// Place signs
	%newSign = new StaticShape(NorthIFLSign) 
	{
		position	= vectorAdd(CentralDropPoint.getPosition(), "0 212 100");
		scale		= "5 1 5";
		datablock	= NorthSign;
	};
	ObjectMarkers.add(%newSign);
	%newSign = new StaticShape(SouthIFLSign) 
	{
		position	= vectorAdd(CentralDropPoint.getPosition(), "0 -212 100");
		scale		= "5 1 5";
		datablock	= SouthSign;
	};
	ObjectMarkers.add(%newSign);
	%newSign = new StaticShape(EastIFLSign) 
	{
		position	= vectorAdd(CentralDropPoint.getPosition(), "212 0 100");
		scale		= "5 1 5";
		rotation	= "0 0 -1 90";
		datablock	= EastSign;
	};
	ObjectMarkers.add(%newSign);
	%newSign = new StaticShape(WestIFLSign) 
	{
		position	= vectorAdd(CentralDropPoint.getPosition(), "-212 0 100");
		scale		= "5 1 5";
		rotation	= "0 0 -1 90";
		datablock	= WestSign;
	};
	ObjectMarkers.add(%newSign);
 }
   // Drop the player(s) in the center of the world again.
   %count = ClientGroup.getCount();
   for(%cl= 0; %cl < %count; %cl++)
   {
      switch( %cl )
      {
      case 0:
         %offset = "0 0 0";

      default:
         %offsetX = getRandom(30);
         %offsetY = getRandom(30);
         %offset = %offsetX SPC %offsetY SPC "0";
      }

      %client = ClientGroup.getObject(%cl);
      DropObjectFromMarker( %client.player , CentralDropPoint , %offset );      
   }

   // Start the picking cycle...
   
   // Be sure that every client has right camera and player body.
   %count = ClientGroup.getCount();
   for(%cl= 0; %cl < %count; %cl++)
   {
      %client = ClientGroup.getObject(%cl);
      %client.player.setDataBlock( BasePlayer );
      %client.camera.setDataBlock( BaseCamera );
   }   
}

// Note: This function will not move TSStatic() objects.
//       Their positions must be set once and only once,
//       on construcyion.  Use CalculateObjectDropPosition() instead.
//

function DropObject( %object , %offsetVector ) 
{
   %mask	= 
      $TypeMasks::TerrainObjectType			|
      $TypeMasks::InteriorObjectType			|
      $TypeMasks::StaticShapeObjectType		|
      $TypeMasks::PlayerObjectType			|
      $TypeMasks::ItemObjectType				|
      $TypeMasks::VehicleObjectType			|
      $TypeMasks::VehicleBlockerObjectType	|
      $TypeMasks::StaticTSObjectType;

   %oldTransform = %object.getTransform();
   %oldPosition  = getWords(%oldTransform, 0, 2);
   %subTerrain   = getWords(%oldTransform, 0, 1) @ " -1";

   %newPosition = containerRayCast( %oldPosition , %subTerrain , %mask , 0 );

   	//echo("Ray hit => " @ %newPosition);

   %newPosition = getWords(%newPosition, 1 , 3);
   
   // Add user specified offset
   if("" !$= %offsetVector) 
   {
      %newPosition = VectorAdd(%newPosition, %offsetVector);
   }

   %newTransform = %newPosition SPC getWords( %oldTransform , 3 , 6 );

   //echo("\c2" SPC %object.getName() @ " => oldTransform = " @ %oldTransform);
   //echo("\c5" SPC %object.getName() @ " => newTransform = " @ %newTransform);


   // Update this object's position.
   //
   // Q: Why not just do a %obj.position = "x y z"; ??
   // A: This will move the object, but only setTransform()
   //    will correctly update the location/orientation of
   //    the object's bounding/world box.  
   //

   %object.setTransform( %newTransform );

}

function DropObjectFromMarker( %object , %marker , %offsetVector ) {
   %object.setTransform(%marker.getTransform());
   DropObject( %object, %offsetVector ) ;
}


// Note: This function is used to calculate the final position as if a
//       drop were done.

function CalculateObjectDropPosition( %oldPosition , %offsetVector ) {
   %mask	= 
      $TypeMasks::TerrainObjectType			|
      $TypeMasks::InteriorObjectType			|
      $TypeMasks::StaticShapeObjectType		|
      $TypeMasks::PlayerObjectType			|
      $TypeMasks::ItemObjectType				|
      $TypeMasks::VehicleObjectType			|
      $TypeMasks::VehicleBlockerObjectType	|
      $TypeMasks::StaticTSObjectType;

   %subTerrain  = getWords(%oldPosition, 0, 1) @ " -1";

   %newPosition = containerRayCast( %oldPosition , %subTerrain , %mask , 0 );

   %newPosition = getWords(%newPosition, 1 , 3);

   // Add user specified offset
   if("" !$= %offsetVector) {
      %newPosition = VectorAdd(%newPosition, %offsetVector);
   }

   return %newPosition;
}


