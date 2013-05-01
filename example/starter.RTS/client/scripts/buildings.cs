// basic array to hold information about your buildings
// the client is interested in the images only

// $Gui::imagePath is the path from ~ leading to your images. In stock RTS SK, it should
// be set to:
// $Gui::imagePath = "./";
// the rest of the paths are self-generating

   $Gui::imagePath = "starter.RTS/client/ui/";
   $Gui::markerPath = "starter.RTS/data/shapes/";


   $Gui::unitImagePath = $Gui::imagePath @ "buildingDisplay/";
   $Gui::buildingImagePath = $Gui::imagePath @ "buildingDisplay/";
   $Gui::resourceImagePath = $Gui::imagePath @ "resourceDisplay/";
   
   $Gui::BuildingActionCount = 3;
   $Gui::TotalNumberBuildings = 6;
// this is a simple way to keep track of all your building information in one place. It's not the -best- way

// Factory: this building produces wood
   $BD_buildingName[0]     = "Factory";
   $BD_ActionType[0]       = "Harvest";
   $BD_buildingIcon[0]     = $Gui::buildingImagePath @ "icon_factory";
   $BD_ActionIcon[0,0]    = $Gui::resourceImagePath @ "icon_wood";
   $BD_ActionIcon[1,0]   = $Gui::resourceImagePath @ "icon_wood";
   $BD_ActionIcon[2,0]   = $Gui::resourceImagePath @ "icon_wood";
   $BD_Marker_scale[0,0]        = "1 1 1";
   $BD_Marker_thedatablock[0,0] = "factoryBlock";
   $BD_Marker_shapeName[0,0]    = $Gui::markerPath @ "factory/factory.dts";
   $BD_Marker_TweakZAxis[0,0] = "0";

   
// Barracks: this unit hires troops
   $BD_buildingName[1]     = "Barracks";
   $BD_ActionType[1]       = "Hire bonecracker shocker rifleman";
   $BD_buildingIcon[1]     = $Gui::buildingImagePath @ "icon_barracks";
   $BD_ActionIcon[0,1]      = $Gui::unitImagePath @ "train_0";
   $BD_ActionIcon[1,1]      = $Gui::unitImagePath @ "train_1";
   $BD_ActionIcon[2,1]      = $Gui::unitImagePath @ "train_2";
   $BD_Marker_scale[0,1]        = "1 1 1";
   $BD_Marker_thedatablock[0,1] = "barracksBlock";
   $BD_Marker_shapeName[0,1]    = $Gui::markerPath @ "barracks/barracks.dts";
   $BD_Marker_TweakZAxis[0,1] = "0.0";

//Shop: this unit mines. Mines mines MINES! (ok, actually it generates gold)
   $BD_buildingName[2]     = "Shop";
   $BD_ActionType[2]       = "Harvest";
   $BD_buildingIcon[2]     = $Gui::buildingImagePath @ "icon_shop";
   $BD_ActionIcon[0,2]      = $Gui::resourceImagePath @ "icon_gold";
   $BD_ActionIcon[1,2]      = $Gui::resourceImagePath @ "icon_gold";
   $BD_ActionIcon[2,2]      = $Gui::resourceImagePath @ "icon_gold";
   $BD_Marker_scale[0,2]        = "1 1 1";
   $BD_Marker_thedatablock[0,2] = "shopBlock";
   $BD_Marker_shapeName[0,2]    = $Gui::markerPath @ "factory/factory.dts";
   $BD_Marker_TweakZAxis[0,2] = "4.0";


// Farm: this unit produces food
   $BD_buildingName[3]     = "Farm";
   $BD_ActionType[3]       = "Harvest";
   $BD_buildingIcon[3]     = $Gui::buildingImagePath @ "icon_factory";
   $BD_ActionIcon[0,3]      = $Gui::resourceImagePath @ "icon_food";
   $BD_ActionIcon[1,3]      = $Gui::resourceImagePath @ "icon_food";
   $BD_ActionIcon[2,3]      = $Gui::resourceImagePath @ "icon_food";
   $BD_Marker_scale[0,3]        = "1 1 1";
   $BD_Marker_thedatablock[0,3] = "farmBlock";
   $BD_Marker_shapeName[0,3]    = $Gui::markerPath @ "factory/factory.dts";
   $BD_Marker_TweakZAxis[0,3] = "0";

// Foundry: this unit mines. Mines mines MINES! -- stone
   $BD_buildingName[4]     = "Foundry";
   $BD_ActionType[4]       = "Harvest";
   $BD_buildingIcon[4]     = $Gui::buildingImagePath @ "icon_foundry";
   $BD_ActionIcon[0,4]      = $Gui::resourceImagePath @ "icon_stone";
   $BD_ActionIcon[1,4]      = $Gui::resourceImagePath @ "icon_stone";
   $BD_ActionIcon[2,4]      = $Gui::resourceImagePath @ "icon_stone";
   $BD_Marker_scale[0,4]        = "1 1 1";
   $BD_Marker_thedatablock[0,4] = "foundryBlock";
   $BD_Marker_shapeName[0,4]    = $Gui::markerPath @ "foundry/foundry.dts";
   $BD_Marker_TweakZAxis[0,4] = "0";

// Town Center: train peons, drop off point for resources
   $BD_buildingName[5]     = "TownCenter";
   $BD_ActionType[5]       = "Hire villager";
   $BD_buildingIcon[5]     = $Gui::buildingImagePath @ "icon_factory";
   $BD_ActionIcon[0,5]      = $Gui::unitImagePath @ "peon";
   $BD_ActionIcon[1,5]      = $Gui::unitImagePath @ "not_used";
   $BD_ActionIcon[2,5]      = $Gui::unitImagePath @ "not_used";
   $BD_Marker_scale[0,5]        = "1 1 1";
   $BD_Marker_thedatablock[0,5] = "townCenterBlock";
   $BD_Marker_shapeName[0,5]    = $Gui::markerPath @ "foundry/foundry.dts";
   $BD_Marker_TweakZAxis[0,5] = "0";
   
// NOTE: You must also match the datablock's declared RTSUnitTypeName to an index
//       in playGui.cs, function GuiRTSTSCtrl::getTypeID

// we use the technique the RTS SK demonstrates regarding "dynamic" gui controls,
// and build ourselves a BuildingDisplay and BuildingMenu dynamically refreshed
// set of gui controls for displaying and interacting with buildings.


function GuiRTSTSCtrl::initBD_ActionIcons(%this)
{
     $BDActionCount = 0;
     while(isObject(%idx = %this.getBDAction($BDActionCount)))
     {
        $BDActionsArray[$BDActionCount] = %idx;
 echo("initBD_Actions--$BDActionsArray[" @ $BDActionCount @ "] is" SPC %idx);        
        $BDActionCount++;
     }
// echo("   GuiRTSTSCtrl::initBD_ActionIcons--leaving, $BDActionCount is (" @ $BDActionCount++ @ ")");
}

function GuiRTSTSCtrl::getBDAction(%this, %Num)
{
   %ret = "BM_Action_" @ %Num;
   return %ret;
}
   
// The following 3 methods are used to index our data with various input and output variables
// This is a very hard to maintain implementation, and a good suggestion would be to re-factor
// so that all of the data is stored in the building information arrays above. What I suggest is
// a very basic abstraction of array access, but this does work for the purposes of the demo resource.

// NOTE: As mentioned elsewhere, the example SelectionDisplay provided by the RTS SK makes a bad assumption:
// dataBlocks (specifically dataBlock names) are NOT actually accessable to the client in a remote situation.
// dataBlocks are actually only stored in the server's process(es), and the most proper implementation for
// indexing dataBlocks via their "name" requires code changes. We use the technique the RTS SK does, and
// make the (faulty) assumption that we can always access the server's dataBlock for a particular object
// via %objID.getDataBlock().getName() to keep our "script only" implementation
   
function getBuildingTypeIDFromUnitName(%unitName)
{
   switch$(%unitName)
   {
      case "factory":
         return 0;
      case "testbuilding":
         return 0;
      case "barracks":
         return 1;
      case "shop":
         return 2;
      case "farm":
         return 3;
      case "foundry":
         return 4;
      case "townCenter":
         return 5;
   }
}  

function getBuildingTypeIDFromDataBlock(%unitDB)
{
   switch$(%unitDB)
   {
      case "factoryBlock":
         return 0;
      case "testbuildingBlock":
         return 0;
      case "barracksBlock":
         return 1;
      case "shopBlock":
         return 2;
      case "farmBlock":
         return 3;
      case "foundryBlock":
         return 4;
      case "townCenterBlock":
         return 5;
   }
}  
function getBuildingTypeID(%objID)      
{
  return (getBuildingTypeIDFromDataBlock(%objID.getDataBlock().getName() ) );
}  

function GuiRTSTSCtrl::setBuildCommandState(%this, %state )
{
   %building = $BuildingMenu::CurrentSelectedBuilding;
   if (!isObject(%building))
   {
     echo("GuiRTSTSCtrl::setBuildCommandState--$BuildingMenu::CurrentSelectedBuilding NOT set. Fix this in selection.cs");
     return;
   }
    
   if ((%building.currentCommand $= "None") ||
       (%building.currentCommand $= ""    ) )
   {
     echo("GuiRTSTSCtrl::setBuildCommandState--Changing building (" @ %building @ ") command state to: " @ %state);
     switch$(%state)
     {
       case "bonecracker":
         %building.currentCommand = "Train bonecracker";
         commandToServer('QueueTrainUnit', LOCAL, 
                         0);
       case "shocker":
         %building.currentCommand = "Train shocker";
         commandToServer('QueueTrainUnit', LOCAL,
                         1);
       case "rifleman":
         %building.currentCommand = "Train rifleman";
         commandToServer('QueueTrainUnit', LOCAL, 
                         2);
       case "villager":
         %building.currentCommand = "Train villager";
         commandToServer('QueueTrainUnit', LOCAL,
                         3);
     }
   }
   else
   {
     echo("GuiRTSTSCtrl::setBuildCommandState--currently ignoring new build order: processing" SPC 
         %building.currentCommand SPC "at this time.");
   }
     
     
}
function clientCmdInitBuildMenuStatusBar( %building, %actionDuration )
{
  // this -may- not be the best way, but we want to know
  // how long the total current action's total time will be,
  // and that information is always stored server side. Have them
  // tell us to init the bar, and we'll process from there
  
  %activeBuilding = ServerConnection.resolveGhostID(%building);
  %activeBuilding.startActionTime = getSimTime();
  %activeBuilding.actionDuration = %actionDuration;
  %activeBuilding.actionApproved = "true";
}

  
function GuiRTSTSCtrl::refreshBuildingDisplay(%this, %repeat)
{
  %activeBuilding = $BuildingMenu::CurrentSelectedBuilding;
   if( (%repeat) &&
       ( isObject(%activeBuilding) ) )

      %this.schedule(100,"refreshBuildingDisplay",true);      
  %activeBuilding = $BuildingMenu::CurrentSelectedBuilding;
  %buildingIndex = getBuildingTypeID(%activeBuilding);

   // Show all portraits
   BD_Selected_Image.setBitMap($BD_BuildingIcon[%buildingIndex]);
   BD_Selected_Image.visible = "1";
   Building_Background.visible = "1";
}
function GuiRTSTSCtrl::updateBuildingStatusBar(%building)
{
  %statusPercent = (getSimTime() - %building.startActionTime) / %building.actionDuration;
// echo("GuiRTSTSCtrl::updateBuildingStatusBar--progress for (" @ %building @ ") is (" @ %statusPercent @ ")");
  return %statusPercent;
  
}
function GuiRTSTSCtrl::refreshBuildingMenu(%this, %repeat)
{
// need to make sure this is called when it should be repeated, and not when it's not
// probably just cancel the schedule on shutdown of PlayGui
// the current resource's implentation works, but is probably not the most performance
// efficient implementation. In a "perfect" implementation, instead of scheduling a refresh
// every 10th of a second, we would have an event driven refresh. In keeping with the technique the
// RTS-SK uses, we stick with a scheduled refresh implementation

// echo("CurrentSelectedBuilding = (" @ $BuildingMenu::CurrentSelectedBuilding @ ")");
  %activeBuilding = $BuildingMenu::CurrentSelectedBuilding;

   if( (%repeat) &&
       ( isObject(%activeBuilding) ) )
   {
      %this.schedule(100,"refreshBuildingMenu",true);      
   }
   else
   {
    return;
   }
// echo("CurrentSelectedBuilding = (" @ $BuildingMenu::CurrentSelectedBuilding @ ")");
  %buildingIndex = getBuildingTypeID(%activeBuilding);
  
  if (!isObject(%activeBuilding) )
  {
    // what are we doing here? turn everything off
   echo("GuiRTSTSCtrl::refreshBuildingMenu==no selected building!");    
   BuildingMenu.visible = "0";
   BuildingDisplay.visible = "0";
//   %this.cancel();
   return;
  }
  
// spin in our action icons 
  for (%i = 0; %i < $Gui::BuildingActionCount; %i++)
  {
    $BDActionsArray[%i].setBitMap($BD_ActionIcon[%i,%buildingIndex]);
    // until we have upgradeable buildings, turn 'em all on
    $BDActionsArray[%i].visible = "1";
    if (getWord($BD_ActionType[%buildingIndex],0) $= "Hire") 
    {
      // hack to only show 3 icons if we're training. Different state checks here
      // can be applied for different building types, including upgrade status/level
      // to turn off a particular action icon as appropriate
      if ($BD_ActionIcon[%i,%buildingIndex] $= ($Gui::unitImagePath @ "not_used"))
      {
        $BDActionsArray[%i].visible = "0";
      }
      else
      {
        $BDActionsArray[%i].visible = "1";
      }
    }
    else
    {
      $BDActionsArray[%i].visible = ( (%i == 0) ? "1" : "0");
    }
  }
   // Show all portraits
   BM_Background.visible = "1";

  if ( ( getWord(%activeBuilding.currentCommand,0) $= "Train" )&&
       ( %activeBuilding.actionApproved $= "true")  )
  {
// echo("GuiRTSTSCtrl::refreshBuildingMenu--current command is (" @ 
//    %activeBuilding.currentCommand @ ") for (" @ %activeBuilding @ ")");
// echo( "-----Unit index is (" @ getWord(%activeBuilding.currentCommand, 1) @ ")");
    BM_CurActionPortrait.setBitmap( %this.getBitmapFromUnitName(getWord(%activeBuilding.currentCommand,1)) );
    BM_CurActionPortrait.visible = "1";
    %curStatus = GuiRTSTSCtrl::updateBuildingStatusBar(%activeBuilding);
    BM_CurActionStatus.visible = "1";
    // here is where we update the value for our visual status bar
    BM_CurActionStatus.setValue(%curStatus);
  }
  else
  {
    // we're not hiring any units at this time, so no need to display the Action Icon
    // or progress bar. Of course, if you implement new building actions (such as upgrade, or
    // research), you'll want to be able to display the progress. Do it here, as well as 
    // implement the required support functions both server side and client side for upgrade/train
    // initialization and progress
    BM_CurActionPortrait.visible = "0";
    BM_CurActionPortrait.setBitMap("");
    BM_CurActionStatus.visible = "0";
    BM_CurActionStatus.setValue("");
  }
}
function clientCmdBuildUnitDenied( %building )
{
  %activeBuilding = ServerConnection.resolveGhostID(%building);
  %activeBuilding.currentCommand = "None";
  %activeBuilding.actionDuration = "0";
  %activeBuilding.curActionStartTime = "0";
  
}
function clientCmdBuildUnitComplete( %building )
{
//echo("clientCmdBuildUnitComplete-- server said we are done with current command for (" @ %building @ ")");
 // we're done with that action, look for any pending trains (not implemented)
  // otherwise set state to None
  %activeBuilding = ServerConnection.resolveGhostID(%building);
//echo("clientCmdBuildUnitComplete-- resolved that id to local ghostID (" @ %activeBuilding @ ")");
  
  %activeBuilding.currentCommand = "None";
  %activeBuilding.actionApproved = "false";
  BM_CurACtionStatus.setValue(0);
  BM_CurActionStatus.visible = "0";
  BM_CurActionPortrait.visible = "0";
}

function startPlaceBuilding(%buildingName)
{
   // Building is already being placed
   if( isObject( $NewBuilding ) )
      return;
   %buildingIndex = getBuildingTypeIDFromUnitName(%buildingName);
   echo("startPlaceBuilding--building name is (" @ %buildingName @ ") building index is (" @ %buildingIndex @ ")");
   $NewBuilding = new RTSBuildingMarker()
   {
      scale = $BD_Marker_scale[0,%buildingIndex];
      thedatablock = $BD_Marker_thedatablock[0,%buildingIndex];
      shapeName = $BD_Marker_shapeName[0,%buildingIndex];
   };
   // Starting position = (0,0,0) 
   $NewBuilding.setTransform( "0 0 0 0 0 1 3.14159" ); 
   $NewBuilding.setOverrideTexture("starter.RTS/data/shapes/factory/building_green");
   PlayGui.startBuildingPlacement($NewBuilding);
}

function GuiRTSTSCtrl::placeBuilding(%this)
{
// right now, buildings can be placed by peons basically anywhere, regardless of
// where the peon is. A good task here would be to track where the selected building position
// is, and make sure that the peon assigned to do the build moves to that spot before
// we actually cause the building to be placed. Not currently implemented

   echo("Client sending building notify!" SPC $NewBuilding.getTransform() SPC $NewBuilding.thedatablock);
   commandToServer('PlaceBuilding', "LOCAL", $NewBuilding.getTransform(), 
                    $NewBuilding.thedatablock);
   $NewBuilding.delete();
}

