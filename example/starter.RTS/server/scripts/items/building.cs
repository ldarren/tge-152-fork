// map some server side building qualities
// note: for expansion purposes, we make this 2d:
//  first index is the building index, second is the building level
// this isn't perfect by any means, but it does give you a direction to go in
// for upgradeable buildings

// factory
$Buildings::requiredBuildSupplies[0,0] = "Gold 250 Stone 45";
$Buildings::requestId[0,0]             = "PlaceFactoryBuilding";
$Buildings::ActionType[0,0]            = "Supply Wood 10 10";

// shop
$Buildings::requiredBuildSupplies[1,0] = "Gold 300 Wood 75";
$Buildings::requestId[1,0]             = "PlaceShopBuilding";
$Buildings::ActionType[1,0]           = "Supply Gold 50 8";

// barracks 
$Buildings::requiredBuildSupplies[2,0] = "Gold 200 Wood 50 Stone 100";
$Buildings::requestId[2,0]             = "PlaceBarracksBuilding";
$Buildings::ActionType[2,0]           = "Hire bonecracker shocker rifleman"; // only the "hire" portion is implemented

// farm
$Buildings::requiredBuildSupplies[3,0] = "Gold 150 Wood 75";
$Buildings::requestId[3,0]             = "PlaceFarmBuilding";
$Buildings::ActionType[3,0]           = "Supply Food 5 12";

// foundry
$Buildings::requiredBuildSupplies[4,0] = "Gold 350 Stone 45 Wood 20";
$Buildings::requestId[4,0]             = "PlaceFoundryBuilding";
$Buildings::ActionType[4,0]           = "Supply Stone 10 12";

// town center
$Buildings::requiredBuildSupplies[5,0] = "Gold 350 Stone 45 Wood 20";
$Buildings::requestId[5,0]             = "PlaceTownCenterBuilding";
$Buildings::ActionType[5,0]           = "Hire villager";




// NOTE: as described in /client/scripts/buildings.cs, this is not a clean
// implementation. All this information should be stored in a script array
// and get methods made for each input/output combination, instead of
// manually copying the information in each method

function getBuildingIndexFromDataBlockName( %datablockName )
{
  // simple indexer to map datablock names to an index for use in arrays
  switch$(%datablockName)
  {
    case "TestBuildingBlock":
      return 0;
    case "factoryBlock":
      return 0;
    case "shopBlock":
      return 1;
    case "barracksBlock":
      return 2;
    case "farmBlock":
      return 3;
    case "foundryBlock":
      return 4;
    case "townCenterBlock":
      return 5;
  }
}

function getBuildingIndexFromUnitTypeName( %unitTypeName )  
{
  // simple indexer to map rts unit type names (only thing client knows about)
  // to an index for use in arrays
  switch$(%UnitTypeName)
  {
    case "testbuilding":
      return 0;
    case "factory":
      return 0;
    case "shop":
      return 1;
    case "barracks":
      return 2;
    case "farm":
      return 3;
    case "foundry":
      return 4;
    case "townCenter":
      return 5;
  }
}

function getDataBlockFromUnitTypeName( %unitTypeName )  
{
  // simple indexer to map rts unit type names (only thing client knows about)
  // to an index for use in arrays
  switch$(%datablockName)
  {
    case "testbuilding":
      return "TestBuildingBlock";
    case "factory":
      return "factoryBlock";
    case "shop":
      return "shopBlock";
    case "barracks":
      return "barracksBlock";
    case "farm":
      return "farmBlock";
    case "foundry":
      return "foundryBlock";
    case "townCenter":
      return "townCenterBlock";
  }
}

// NOTE: The above three functions MUST be refactored

datablock RTSUnitData(TestBuildingBlock : UnitBaseBlock)
{
   shapeFile = "~/data/shapes/factory/factory.dts";
//-----------------------Begin Bug Fix: http://www.garagegames.com/mg/forums/result.thread.php?qt=23223
   //RTSUnitTypeName = "building";
//-----------------------End Bug Fix 
   RTSUnitTypeName = "factory"; 
   boundingBox = "10.0 10.0 3.0";
};



datablock RTSUnitData(factoryBlock : UnitBaseBlock)
{
   shapeFile = "~/data/shapes/factory/factory.dts";
   RTSUnitTypeName = "factory";   
   boundingBox = "10.0 10.0 3.0";
};



datablock RTSUnitData(barracksBlock : UnitBaseBlock)
{
   shapeFile = "~/data/shapes/barracks/barracks.dts";
   RTSUnitTypeName = "barracks";
   boundingBox = "10.0 10.0 3.0";
};

datablock RTSUnitData(shopBlock : UnitBaseBlock)
{
   shapeFile = "~/data/shapes/shop/shop.dts";
   RTSUnitTypeName = "shop";
   boundingBox = "10.0 10.0 3.0";
};

datablock RTSUnitData(farmBlock : UnitBaseBlock)
{  
   shapeFile = "~/data/shapes/factory/factory.dts";
   RTSUnitTypeName = "farm";
   boundingBox = "10.0 10.0 3.0";
};

datablock RTSUnitData(foundryBlock : UnitBaseBlock)
{  
   shapeFile = "~/data/shapes/foundry/foundry.dts";
   RTSUnitTypeName = "foundry";
   boundingBox = "10.0 10.0 3.0";
};

datablock RTSUnitData(townCenterBlock : UnitBaseBlock)
{
   shapeFile = "~/data/shapes/foundry/foundry.dts";
   RTSUnitTypeName = "townCenter";
   boundingBox = "10.0 10.0 3.0";
};

function RTSUnit::initBuildingActions(%this)
{
  // handler to hook in functionality of a building from it's base infoset
echo("RTSUnit::initBuildingActions");
  %buildingIndex = getBuildingIndexFromDataBlockName(%this.getDataBlock().getName());
  %actionList = $Buildings::ActionType[%buildingIndex,0]; // note hardcoded 0, building levels not implemented
  %actionType = getWord(%actionList, 0); // grab the first word in the info string, it's what the building does
echo("RTSUnit::initBuildingActions--index is (" @ %buildingIndex @ 
") actionList is (" @ %actionList @ ") Action type is (" @ %actionType @ ")");

  switch$(%actionType)  
  {
    case "Shop" : // not implemented
      return;
    case "Supply":
//    {
      %suppliesString = getWords(%actionList, 1); // grab everything after the type
      // parse it out
      for (%supplyIndex = 0; 1 ; %supplyIndex++)
      {
        
        %supplyToAdd = getWords(%suppliesString, %supplyIndex * 3,
                               (%supplyIndex *3) + 1);
        echo("inside action type, parsing type (" @ %actionType @ "), supplies to add (" @
             %supplyToAdd @ ")");
        if (%supplyToAdd $= "")
          break; // no more supplies to parse
        %supplyScheduleTime = getWord(%suppliesString, (%supplyIndex * 3) + 2); 
        repeatingAddSupplies(%this, %this, (%supplyScheduleTime * 1000), "LOCAL",
        "ScheduledRepeatingSupplyAdd", %supplyToAdd, "true");
      }
    case "Train": // functionality is implemented via several client side
                  // and server side commandToClient/commandToServer calls.
                  // would be best to re-factor and implement here if possible. 
      return;
  } // end switch
}  

  
function serverCmdPlaceBuilding(%conn, %store, %transform, %data, %zTweak)
{
  echo("serverCmdPlaceBuilding--client(" @ %conn @ ") building (" @  
       %data @ ")\n transform (" @ %transform @ ") Tweak (" @ %zTweak @ ")");
   //TODO: do some checks to verify that we can place a building here
   
   // first, check to see if we have the right supplies
   if (%store $= "LOCAL")
   {
     %activeStore = %conn.resourceStore;
   }
   else
   {
     %activeStore = %store;
   }
   
   %requiredSupplies = $Buildings::requiredBuildSupplies[(getBuildingIndexFromDataBlockName( %data )),0];
   %requestId = $Buildings::requestId[(getBuildingIndexFromDataBlockName( %data )),0];
   
   %authString = resourceStore::requestSpendSupplies(%conn,
                                                     %activeStore,
                                                     %requestId, 
                                                     %requiredSupplies, 
                                                     "false");

   %successStatus = getWord(%authString,0);
   if (%successStatus $= "DENY") 
   {
//   	echo("serverCmdPlaceBuilding--request by (" @ %conn @ ")"  SPC getWord(%authString, 1) SPC "DENIED");
     messageClient(%conn, 'MsgPurchaseDenied', "", "Cannot place Building! missing:" SPC getWords(%authString, 2) );
   	return;
  }
  else
  {
//  	echo("serverCmdPlaceBuilding--request by" SPC %client SPC getWord(%authString, 1) SPC "APPROVED");
  }
   %b = new RTSBuilding()
   {
      datablock = %data;
      scale = "1 1 1";
   };
   %b.setTransform( %transform );
   
   %b.client = %conn;
   %b.setTeam(%conn.getTeam());
   %b.setControllingConnection(%conn);
   %conn.buildings.add(%b);
   
   %b.playRootAnimation();
   
//-----------------------Begin Bug Fix:  http://garagegames.com/mg/forums/result.thread.php?qt=23324  
   //MissionCleanup.add(%b);
//-----------------------End Bug Fix 

   // Note: using schedule is a very basic example. In a real game, adding the supply
   // to the store would probably be dependent on some form of game event, like
   // a working returning a load of carried supplies to a player's building
   // for example, how the villagers work. As you increase your building's capabilities,
   // most (if not all) will not be supply type buildings.
  echo("serverCmdPlaceBuilding()--building (" @ %b @ "):");
  %b.initBuildingActions();
}

function serverCmdQueueTrainUnit(%client, %store, %unitType)
{

 	echo("serverCmdQueueTrainUnit--request by (" @ %client @ ") unit type"  SPC %unitType);


  // first, see if we have a good building selected
  if (%client.selection.getCount() > 1) 
  {
    error("serverCmdQueueTrainUnit--client (" @ %client @ ") had more than one selection in selection group!");
    return;
  }
  %activeBuilding = %client.selection.getObject(0);
   
   // second, check to see if we have the right supplies
   switch$(%unitType)
   {
     case "0" :
       %requiredSupplies = "Gold 200 Stone 15";
       %requestId = "QueueTrainBonecracker";
       %trainDuration = 5000;
     case "1" :
       %requiredSupplies = "Gold 250 Wood 15";
       %requestId = "QueueTrainShocker";
       %trainDuration = 6500;
     case "2" :
       %requiredSupplies = "Gold 350 Wood 10 Stone 10";
       %requestId = "QueueTrainRifleman";
       %trainDuration = 8000;
     case "3" :
       %requiredSupplies = "Gold 200";
       %requestId = "QueueTrainVillager";
       %trainDuration = 4500;
   }
      
   if (%store $= "LOCAL")
   {
     %activeStore = %client.resourceStore;
   }
   else
   {
     %activeStore = %store;
   }
   %authString = resourceStore::requestSpendSupplies(%client,
                                                     %activeStore,
                                                     %requestId, 
                                                     %requiredSupplies, 
                                                     "false");

   %successStatus = getWord(%authString,0);
   if (%successStatus $= "DENY") 
   {
//   	echo("serverCmdPlaceBuilding--request by (" @ %conn @ ")"  SPC getWord(%authString, 1) SPC "DENIED");
     messageClient(%client, 'MsgPurchaseDenied', "", "Cannot train unit! missing:" SPC getWords(%authString, 2) );
     commandToClient(%client, 'BuildUnitDenied', %client.getGhostId(%activeBuilding) );

   	return;
  }
  else
  {
//  	echo("serverCmdPlaceBuilding--request by" SPC %client SPC getWord(%authString, 1) SPC "APPROVED");
  }

  %activeBuilding.TrainUnitEventId = schedule(%trainDuration,
                                     %activeBuilding,
                                     "trainUnitDurationComplete", 
	                                   %client, %activeBuilding, %unitType);
  echo("serverCmdQueueTrainUnit--queued up unit type" SPC %unitType SPC 
       "at builiding (" @ %activeBuilding @ ") for client (" @ %client @ "), duration" @
       %trainDuration );
  commandToClient(%client, 'InitBuildMenuStatusBar', %client.getGhostId(%activeBuilding),
                   %trainDuration );
}
  
function trainUnitDurationComplete(%client, %building, %unitType)
{
// 	echo("trainUnitDurationComplete--request by (" @ %client @ ") unit type" SPC 
// 	   %unitType SPC "placing near building (" @ %building @ ")");

  // ok, unit is paid for, training time is complete, let's hand 'em over!
  // figure out a spawn point here.
  %spawnCenter = %building.getPosition();
  %spawnOffset = "-8 -8 0";
  %spawnPoint = VectorAdd( %spawnCenter, %spawnOffset); 
  %client.createPlayer( getWords(%spawnPoint,0,2), %unitType);
  messageClient(%client, 'MsgUnitComplete', "", %spawnPoint);
  commandToClient(%client, 'BuildUnitComplete', %client.getGhostId(%building) );
}
