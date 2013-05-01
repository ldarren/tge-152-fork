function GuiRTSTSCtrl::onMouseOverUnit(%this, %obj)
{
}

function GuiRTSTSCtrl::onMouseOverTerrain(%this)
{
}

function GuiRTSTSCtrl::onMousePanDisplay(%this, %enable, %flags)
{
   if(%flags & 2)
      moveleft(%enable);
   if(%flags & 4)
      moveright(%enable);
   if(%flags & 8)
      moveforward(%enable);
   if(%flags & 16)
      movebackward(%enable);
}

function GuiRTSTSCtrl::onMouseDown(%this, %obj, %x, %y, %z)
{
   //echo(%obj.getClassName() @ "clicked on");
   switch$(%obj.getClassName())
   {
      case "RTSUnit":
         %this.onMouseDownUnit(%obj);
      case "TerrainBlock":
         %this.onMouseDownTerrain(%x,%y,%z);
      case "RTSBuilding":
         %this.onMouseDownBuilding(%obj);
   }
}

function GuiRTSTSCtrl::onMouseDownUnit(%this, %unit)
{
   switch$($CommandMenu::CurrentCommand)
   {
      case "Attack":
         if(%this.selectionIncludesTeam)
            commandToServer('IssueAttack', %unit.getGhostID());
         %this.setCommandState("None");
      case "BuffPlus":
         if(%this.selectionIncludesTeam)
            commandToServer('ApplyModifier', "Bonus", %unit.getGhostID(), 10000);
         %this.setCommandState("None");
      case "BuffMinus":
         if(%this.selectionIncludesTeam)
            commandToServer('ApplyModifier', "Handicap", %unit.getGhostID(), 10000);
         %this.setCommandState("None");
      case "Move":
         if(%this.selectionIncludesTeam)
         {
            %destination = %unit.getPosition();
            commandToServer('IssueMove',
                              getWord(%destination, 0),
                              getWord(%destination, 1),
                              getWord(%destination, 2));
         }
//-----------------------Begin Bug Fix: http://www.garagegames.com/mg/forums/result.thread.php?qt=67830		 
		 %this.setCommandState("None");
//-----------------------End Bug Fix
      case "None":
         if($Selection::Modifier)
         {
            if(%this.selectionIncludesTeam && (%unit.getTeam() == ServerConnection.getTeam()))
               %this.toggleSelectObject(%unit);
         }
         else
         {
            %this.clearSelection();
            %this.selectObject(%unit);
         }
   }
}

function GuiRTSTSCtrl::onMouseDownTerrain(%this, %x, %y, %z)
{
   switch$($CommandMenu::CurrentCommand)
   {
      case "Move":
         if(%this.selectionIncludesTeam)
            commandToServer('IssueMove', %x, %y, %z);
         %this.setCommandState("None");
//-----------------------Begin Bug Fix:  http://www.garagegames.com/mg/forums/result.thread.php?qt=67724
      case "None":
         %this.clearTheSelection(%this);
//-----------------------End Bug Fix 
   }
}

function GuiRTSTSCtrl::onMouseDownBuilding(%this, %building)
{
//  echo("GuiRTSTSCtrl::onMouseDownBuilding()--command is" SPC $CommandMenu::CurrentCommand);
   switch$($CommandMenu::CurrentCommand)
   {
      case "Attack":
         if(%this.selectionIncludesTeam)
            commandToServer('IssueAttack', %building.getGhostID());
//-----------------------Begin Bug Fix: http://www.garagegames.com/mg/forums/result.thread.php?qt=67830		 
		 %this.setCommandState("None");
//-----------------------End Bug Fix
      case "Move":
         if(%this.selectionIncludesTeam)
         {
            %destination = %unit.getPosition();
            commandToServer('IssueMove',
                              getWord(%destination, 0),
                              getWord(%destination, 1),
                              getWord(%destination, 2));
         }
//-----------------------Begin Bug Fix: http://www.garagegames.com/mg/forums/result.thread.php?qt=67830		 
		 %this.setCommandState("None");
//-----------------------End Bug Fix
      case "None":
//-----------------------Begin Bug Fix:  http://www.garagegames.com/mg/forums/result.thread.php?qt=67831
         //if(%this.getSelectionSize())
//-----------------------End Bug Fix
            %this.selectBuilding(%building);
   }
}

function GuiRTSTSCtrl::onRightMouseDown(%this, %obj, %x, %y, %z)
{
   //echo(%obj.getClassName() @ "right clicked on");
   %this.setCommandState("None");
   %this.selectionLocked = false;
   switch$(%obj.getClassName())
   {
      case "RTSUnit":
         %this.onRightMouseDownUnit(%obj);
      case "TerrainBlock":
         %this.onRightMouseDownTerrain(%x,%y,%z);
      case "StaticShape":
		 %this.onRightMouseDownResource(%x,%y,%z,%obj);
   }
}

function GuiRTSTSCtrl::onRightMouseDownResource(%this, %x, %y, %z, %type)
{
   echo("GuiRTSTSCtrl::onRightMouseDownResource--collecting from (" @ %type @ ")");
   if(%this.selectionIncludesTeam)
      commandToServer('IssueResourceMove', %x, %y, %z, %type.getGhostID() );
}

function GuiRTSTSCtrl::onRightMouseDownUnit(%this, %unit)
{
   if(ServerConnection.getTeam() == %unit.getTeam())
   {
      echo("Follow not yet implemented.");
   }
   else
   {
      if(%this.selectionIncludesTeam)
      {
         echo("IssueAttack(" @ %this @ ", " @ %unit.getGhostID() @ ")");
         commandToServer('IssueAttack', %unit.getGhostID());
      }
   }
}

function GuiRTSTSCtrl::onRightMouseDownTerrain(%this, %x, %y, %z)
{
   if(%this.selectionIncludesTeam)
      commandToServer('IssueMove', %x, %y, %z);
}

function GuiRTSTSCtrl::onMouseUp(%this, %isDragSelect, %obj, %x, %y, %z)
{
   if(%isDragSelect)
   {
      if($Selection::Modifier)
      {
         for(%i = 0; %i < %this.getDragSize(); %i++)
         {
            %obj = %this.getDragObject(%i);
            switch$(%obj.getClassName())
            {
               case "RTSUnit":
                  if(%this.selectionIncludesTeam)
                     if(%obj.getTeam() == ServerConnection.getTeam())
                        %this.toggleSelectObject(%obj);

               case "RTSBuilding":
                  if((%this.getSelectionSize() == 0) && (%this.getDragSize() == 1))
                     %this.selectBuilding(%obj);
            }
         }
      }
      else
      {
         for(%i = 0; %i < %this.getDragSize(); %i++)
         {
            %obj = %this.getDragObject(%i);
            switch$(%obj.getClassName())
            {
               case "RTSUnit":
                  if(%this.dragSelectionIncludesTeam)
                  {
		     if(%obj.getTeam() == ServerConnection.getTeam())
		        %this.toggleSelectObject(%obj);
		  }
	          else
                  {
                     %this.selectObject(%obj);
                     break;
                  }
                  
               case "RTSBuilding":
                  if((%this.getSelectionSize() == 0) && (%this.getDragSize() == 1))
                     %this.selectBuilding(%obj);
            }
         }
      }
      %this.clearDrag();
   }
}
