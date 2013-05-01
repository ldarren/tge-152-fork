//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PlayGui is the main TSControl through which the game is viewed.
// The PlayGui also contains the hud controls.
//-----------------------------------------------------------------------------

function GuiRTSTSCtrl::onWake(%this)
{
   $enableDirectInput = "1";
   activateDirectInput();

   // Message hud dialog
   if (!$RunningDemo)
   {
      %this.add(MainChatHud);
      chatHud.attach(HudMessageVector);
   }

   // just update the action map here
   moveMap.push();
   
   // hack city - these controls are floating around and need to be clamped
   schedule(0, 0, "refreshCenterTextCtrl");
   schedule(0, 0, "refreshBottomTextCtrl");
   
   %this.initSDPortraits();
echo("------------------Before initBD_ActionIcons---------------");   
   %this.initBD_ActionIcons();
echo("------------------After initBD_ActionIcons---------------");   
   %this.BuildingDisplay.visible = "0";
   %this.BuildingMenu.visible = "0";
   %this.refreshSelectionDisplay(true);
   %this.refreshBuildingDisplay(true);

   Canvas.cursorOn();
}

function GuiRTSTSCtrl::onSleep(%this)
{
//   PlayGui.remove(MainChatHud);
   
   // pop the keymaps
   moveMap.pop();
}

//-----------------------------------------------------------------------------

function GuiRTSTSCtrl::setCommandState(%this, %state)
{
//   echo("Changing command state to: " @ %state);
   switch$(%state)
   {
      case "Move":
         $CommandMenu::currentCommand = "Move";
         %this.selectionLocked = true;
      case "Stop":
         $CommandMenu::currentCommand = "Stop";
      case "Hold":
         $CommandMenu::currentCommand = "Hold";
      case "BuffPlus":
         $CommandMenu::currentCommand = "BuffPlus";
         %this.selectionLocked = true;
      case "Attack":
         $CommandMenu::currentCommand = "Attack";
         %this.selectionLocked = true;
      case "Patrol":
         $CommandMenu::currentCommand = "Patrol";
         %this.selectionLocked = true;
      case "BuffMinus":
         $CommandMenu::currentCommand = "BuffMinus";
         %this.selectionLocked = true;
      case "None":
         $CommandMenu::currentCommand = "None";
         %this.selectionLocked = false;
   }
}

//-----------------------------------------------------------------------------

function GuiRTSTSCtrl::refreshSelectionDisplay(%this, %repeat)
{
   if(%repeat)
      %this.schedule(100,"refreshSelectionDisplay",true);      

   // Hide all portraits and clear selection to prepare for display
   for(%i=0; %i<$SDPortraitCount; %i++)
      $SDPortraitArray[%i].visible = 0;
   SD_Selection.clear();

   // Strings for textarray
   %SDNum = %SDDam = "";     
   
   // Need to truncate down
   if(%this.getSelectionSize() > $SDPortraitCount)
   {
      %Health[0] = 0;
      %Number[0] = 0;
      
      // Build up info
      for(%i = 0; %i < %this.getSelectionSize(); %i++)
      {
         %type = %this.getTypeID(%i);
         %Number[%type]++;
         %Health[%type] += %this.getDamage(%i);
      }

      // Build strings for textarray
      %i = 0;
      while(%i < $SDPortraitCount)
      {
         if(%Number[%i] == 0)
         {
            %SDDam = %SDDam @ "" TAB "";
            %SDNum = %SDNum @ "" TAB "";
         }
         else
         {
            // Average health
            %Health[%i] /= %Number[%i];         
            %SDDam = %SDDam @ %Health[%i] TAB "";
            %SDNum = %SDNum @ %Number[%i] TAB "";
         }                
         %i++;
      }
      
      // Setup bitmaps
      %currentBitmap = 0;
      for(%i = 0; %i < %this.getSelectionSize(); %i++)
      {
         if(%Number[%i] > 0)
         {
            %bitmap = %this.getSDPortrait(%i);
            %currentBitmap++;
            %bitmap.setBitmap(%this.getBitmapFromID(%i));
            %bitmap.visible = 1;
         }
      }
          
   }
   else // fill it up
   {  
      for(%i = 0; %i < %this.getSelectionSize(); %i++)
      {
         $SDPortraitArray[%i].visible = 1;
         $SDPortraitArray[%i].setBitmap(%this.getBitmapFromID(%this.getTypeID(%i))); 
         %SDNum = %SDNum @ %this.getCountString(%i) TAB "";
         %SDDam = %SDDam @ %this.getDamage(%i) TAB "";
      }                                      
   }
   
   SD_Selection.addRow(0,%SDNum);
   SD_Selection.addRow(1,%SDDam); 
}

function GuiRTSTSCtrl::initSDPortraits(%this)
{
   // Creates an array holding all portraits named "SD_Portrait_#"
   $SDPortraitCount = 0;
   while(isObject(%idx = %this.getSDPortrait($SDPortraitCount)))
   {
      $SDPortraitArray[$SDPortraitCount] = %idx;
      $SDPortraitCount++;
   }
}

function GuiRTSTSCtrl::getSDPortrait(%this, %Num)
{
   %ret = "SD_Portrait_" @ %Num;
   return %ret;
}

function GuiRTSTSCtrl::getDamage(%this, %objID)
{
   if(%objID < %this.getSelectionSize())
   {
      %obj = %this.getSelectedObject(%objID);
      return ((1 - %obj.getDamagePercent()) * %obj.getDataBlock().maxDamage);
   }
   else
      return "";
}

function GuiRTSTSCtrl::getCountString(%this, %objID)
{
   if(%objID < %this.getSelectionSize())
      return "x1";
   else 
      return "";
}

function GuiRTSTSCtrl::getTypeID(%this, %objID)
{
   switch$(%this.getSelectedObject(%objID).getDataBlock().getName())
   {
      case "botBlock":
         return 0;
      case "shockerBlock":
         return 1;
      case "riflemanBlock":
         return 2;
      case "villagerBlock":
         return 3;
      case "factoryBlock":
         return 100;
      case "barracksBlock":
         return 101;
      case "shopBlock":
         return 102;
      case "farmBlock":
         return 103;
      case "foundryBlock":
         return 104;
      case "townCenterBlock":
         return 105;   
   }
}

function GuiRTSTSCtrl::getBitmapFromID(%this, %typeID)
{
   switch(%typeID)
   {
      case 0:
         return "starter.RTS/client/ui/selectionDisplay/boneCrackerPortrait";
      case 1:
         return "starter.RTS/client/ui/selectionDisplay/shockerPortrait"; 
      case 2:
         return "starter.RTS/client/ui/selectionDisplay/riflemanPortrait"; 
      case 3:
         return "starter.RTS/client/ui/selectionDisplay/peonPortrait"; 
      case 100:
         return "starter.RTS/client/ui/selectionDisplay/buildingPortrait";
      case 101:
         return "starter.RTS/client/ui/selectionDisplay/buildingPortrait";
      case 102:
         return "starter.RTS/client/ui/selectionDisplay/buildingPortrait";
      case 103:
         return "starter.RTS/client/ui/selectionDisplay/buildingPortrait";
      case 104:
         return "starter.RTS/client/ui/selectionDisplay/buildingPortrait";
      case 105:
         return "starter.RTS/client/ui/selectionDisplay/buildingPortrait";
         
   }
}

function GuiRTSTSCtrl::getIDfromUnitName(%this, %unitName)
{
//-----------------------Begin Bug Fix: http://www.garagegames.com/mg/forums/result.thread.php?qt=23223
   switch$(%this.getSelectedObject(%objID).getRTSUnitTypeName())
   {
      case "bonecracker":
         return 0;
      case "shocker":
         return 1;
      case "rifleman":
         return 2;
      case "villager":
         return 3;
      default:
         return 0;   
         
   }
//-----------------------End Bug Fix  
}

function GuiRTSTSCtrl::getBitmapFromUnitName(%this, %unitName)
{
  return (%this.getUnitBitmapFromID(%this.getIDfromUnitName(%unitName)));
}
function GuiRTSTSCtrl::getUnitBitmapFromID(%this, %typeID)
{
   switch(%typeID)
   {
      case 0:
         return "starter.RTS/client/ui/selectionDisplay/boneCrackerPortrait";
      case 1:
         return "starter.RTS/client/ui/selectionDisplay/shockerPortrait"; 
      case 2:
         return "starter.RTS/client/ui/selectionDisplay/riflemanPortrait"; 
      case 3:
         return "starter.RTS/client/ui/selectionDisplay/peonPortrait";
   }
}

//-----------------------------------------------------------------------------

function refreshBottomTextCtrl()
{
   BottomPrintText.position = "0 0";
}

function refreshCenterTextCtrl()
{
   CenterPrintText.position = "0 0";
}

//-----------------------------------------------------------------------------

