//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SceneGui is the main TSControl through which the demo is viewed.
// The SceneGui also contains the hud controls.
//-----------------------------------------------------------------------------

function SceneGui::onWake(%this)
{
   // Turn off any shell sounds...
   // alxStop( ... );

   $enableDirectInput = "1";
   activateDirectInput();

   // Update the action map
   moveMap.push();

   // Push the menu system
   //OverlayBuyNow.setVisible(false);
   Canvas.pushdialog(OverlayDlg);
}

function SceneGui::onSleep(%this)
{
   // pop the keymaps
   moveMap.pop();
}

function SceneGui::isNextScene(%this)
{
   return %this.sceneNumber < ("MissionGroup/Scenes".getCount() - 1);
}

function SceneGui::isPrevScene(%this)
{
   return %this.sceneNumber > 0;
}

function SceneGui::setSceneNumber(%this,%sceneNumber)
{
   // Set the current room # and update the arrows
   %this.sceneNumber = %sceneNumber;
   
   // Update the navigation buttons
   BottomDesc.setVisible(true);
   OverlayNextPage.setVisible(true);
   OverlayNextPage.command = %this.isNextScene()? "SceneGui.nextScene();" : "escapeFromGame();";
   OverlayPrevPage.setVisible(true); //%this.isPrevScene());
   OverlayPrevPage.command = "SceneGui.prevScene();";

   // Activate the scene object
   %room = Scene::activateScene(%this.sceneNumber);

   // Extract demo text from room object
   OverlayTitle2.setVisible(false);
   OverlayTitle.setVisible(true);
   OverlayTitle.setText(
      "<just:left><color:000000>" @
      %room.title);
   OverlayDesc.setText(
      "<just:left><color:252525>" @
      %room.description);
}

function SceneGui::nextScene(%this)
{
   if (%this.isNextScene())
      %this.setSceneNumber(%this.sceneNumber + 1);
}

function SceneGui::prevScene(%this)
{
   if (%this.isPrevScene())
      %this.setSceneNumber(%this.sceneNumber - 1);
   else
      escapeFromGame();
}

function SceneGui::getSceneName(%this)
{
   return "MissionGroup/Scenes".getObject(%this.sceneNumber).getName();
}
   

