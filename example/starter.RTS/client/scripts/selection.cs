function GuiRTSTSCtrl::updateDragSelect(%this, %obj)
{
// echo("Gui-updateDragSelect");
   if(isObject(%obj))
      switch$(%obj.getClassName())
      {
         case "RTSUnit":
            %this.dragSelectObject(%obj);
         case "RTSBuilding":
            %this.addToSelection(%obj);
      }
}

function GuiRTSTSCtrl::selectBuilding(%this, %building)
{
// echo("Gui-selectBuilding (" @ %building @ "). Previous building was (" @ $BuildingMenu::CurrentSelectedBuilding @ ")");
   %this.clearSelection();
   %this.addToSelection(%building);
   $BuildingMenu::CurrentSelectedBuilding = %building;
   BuildingDisplay.visible = "1";
   BuildingMenu.visible = "1";
   %this.refreshBuildingDisplay();
   %this.refreshBuildingMenu(true);
//   $BuildingMenu::CurrentSelectedBuilding = %building;
   commandToServer('AddToSelection', %building.getGhostID());
}

function GuiRTSTSCtrl::selectObject(%this,%obj)
{
// echo("Gui-selectObject (" @ %object @ "). Previous building was (" @ $BuildingMenu::CurrentSelectedBuilding @ ")");
   if(!%this.selectionLocked)
   {
      %this.addToSelection(%obj);
      echo ("Telling server to select object local id:" @ %obj @
            " GhostID " @ %obj.getGhostID());
      commandToServer('AddToSelection', %obj.getGhostID());
      if(isObject(%obj))
        switch$(%obj.getClassName())
        {
          case "RTSUnit":
            BuildingDisplay.visible = "0";
            BuildingMenu.visible = "0";
//            CommandMenu.visible = "1";
// add from Anthony for village worker menu. probably not quite right state check here

// ----------  Begin Bug Fixed on the mod's forum thread
//            if(%this.getSelectionSize() == 1)
//              WorkingOrders.visible = "1";
//            else
//              CommandMenu.visible = "1"; 
            if( (%this.getSelectionSize() == 1) &&
                (%obj.getDatablock().getName() $= "villagerBlock") )
            {
              CommandMenu.visible = "0";
              WorkingOrders.visible = "1";
            }
            else
            {
              WorkingOrders.visible = "0";
              CommandMenu.visible = "1";
            }
// ----------  End Bug Fixed on the mod's forum thread
			
            %this.refreshSelectionDisplay();
            SelectionDisplay.visible = "1";
         case "RTSBuilding":
          $BuildingMenu::CurrentSelectedBuilding = %obj;
          %this.refreshBuildingDisplay();
          BuildingDisplay.visible = "1";
          %this.refreshBuildingMenu();
          BuildingMenu.visible = "1";
        }

//      CommandMenu.visible = "1";
//      %this.refreshSelectionDisplay();
//      SelectionDisplay.visible = "1";
      if(%obj.getTeam() == ServerConnection.getTeam())
         %this.selectionIncludesTeam = true;
   }
}

function GuiRTSTSCtrl::unselectObject(%this,%obj)
{
//   echo("Script-unselecting object!");
   if(!%this.selectionLocked)
   {
      echo("Selection NOT locked on unselect");
      %this.removeFromSelection(%obj);
      commandToServer('RemoveFromSelection', %obj.getGhostID());
      if(%this.getSelectionSize() == 0)
      {
         CommandMenu.visible = "0";
         SelectionDisplay.visible = "0";
         WorkingOrders.visible = "0";
         %this.selectionIncludesTeam = false;
      }
      else
         %this.refreshSelectionDisplay();
   }
}

function GuiRTSTSCtrl::toggleSelectObject(%this,%obj)
{
// echo("Gui-toggleSelectObject (" @ %building @ "). Previous building was (" @ $BuildingMenu::CurrentSelectedBuilding @ ")");

   if(!%this.isObjectInSelection(%obj))
      %this.selectObject(%obj);
   else
      %this.unselectObject(%obj);

   if(%this.getSelectionSize() == 1 && %this.getSelectedObject(0).getDatablock().getName() $= "villagerBlock"){
     CommandMenu.visible = "0";
     WorkingOrders.visible = "1";
     }
   else
     WorkingOrders.visible = "0";

}

function GuiRTSTSCtrl::clearTheSelection(%this)
{
// echo("Gui-clearTheSelection");
   if(!%this.selectionLocked)
   {
      %this.clearSelection();
      commandToServer('ClearSelection');
      CommandMenu.visible = "0";
      SelectionDisplay.visible = "0";
      BuildingDisplay.visible = "0";
      BuildingMenu.visible = "0";
      WorkingOrders.visible = "0";
// ----------  Begin Bug Fixed on the wd mod's forum thread http://www.garagegames.com/mg/forums/result.thread.php?qt=23546
      //$BuildingMenu::CurrentBuildingSelected = "";
	  $BuildingMenu::CurrentSelectedBuilding = "";
// ----------  End Bug Fixed on the wd mod's forum thread
      %this.selectionIncludesTeam = false;
   }
}

function GuiRTSTSCtrl::checkDragSelectionModifier(%this)
{
   if(!$Selection::Modifier)
      %this.clearTheSelection();
}

function GuiRTSTSCtrl::dragSelectObject(%this,%obj)
{
   if (%obj.getDamageState() $= "Enabled")
      %this.addToDrag(%obj);
   if(%obj.getTeam() == ServerConnection.getTeam())
      %this.dragSelectionIncludesTeam = true;
}

function GuiRTSTSCtrl::dragUnselectObject(%this,%obj)
{
   %this.removeFromDrag(%obj);
   if(%this.getDragSize() == 0)
      %this.dragSelectionIncludesTeam = false;
}

function GuiRTSTSCtrl::dragClearSelection(%this)
{
   %this.clearDrag();
   %this.dragSelectionIncludesTeam = false;
}

function GuiRTSTSCtrl::onSelectObject(%this, %obj)
{
// echo("Gui-onSelectObject (" @ %obj @ "). Previous building was (" @ $BuildingMenu::CurrentSelectedBuilding @ ")");

   commandToServer('AddToSelection', %obj.getGhostID());
      if(isObject(%obj))
        switch$(%obj.getClassName())
        {
          case "RTSUnit":
            BuildingDisplay.visible = "0";
            BuildingMenu.visible = "0";
            CommandMenu.visible = "1";
            %this.refreshSelectionDisplay();
            SelectionDisplay.visible = "1";
         case "RTSBuilding":
          $BuildingMenu::CurrentSelectedBuilding = %obj;
          %this.refreshBuildingDisplay();
          BuildingDisplay.visible = "1";
          %this.refreshBuildingMenu();
          BuildingMenu.visible = "1";
        }

//   CommandMenu.visible = "1";
//   %this.refreshSelectionDisplay();
//   SelectionDisplay.visible = "1";
}

function GuiRTSTSCtrl::onUnselectObject(%this, %obj)
{
// echo("Gui-onUnselectObject (" @ %obj @ "). Previous building was (" @ $BuildingMenu::CurrentSelectedBuilding @ ")");
  
   commandToServer('RemoveFromSelection', %obj.getGhostID());
   %this.refreshSelectionDisplay();
   if (%this.getSelectionSize() == 0)
   {
      CommandMenu.visible = "0";
      SelectionDisplay.visible = "0";
   }
}

function GuiRTSTSCtrl::onClearSelection(%this)
{
   commandToServer('ClearSelection');
   CommandMenu.visible = "0";
   SelectionDisplay.visible = "0";
   $BuildingMenu::CurrentSelectedBuilding = "";
   BuildingMenu.visible = "0";
   BuildingDisplay.visible = "0";
}