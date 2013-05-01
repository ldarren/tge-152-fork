//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Hard coded images referenced from C++ code
//------------------------------------------------------------------------------

//   editor/SelectHandle.png
//   editor/DefaultHandle.png
//   editor/LockedHandle.png


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Mission Editor 
//------------------------------------------------------------------------------

function Editor::create()
{
   // Not much to do here, build it and they will come...
   // Only one thing... the editor is a gui control which
   // expect the Canvas to exist, so it must be constructed
   // before the editor.
   new EditManager(Editor)
   {
      profile = "GuiContentProfile";
      horizSizing = "right";
      vertSizing = "top";
      position = "0 0";
      extent = "640 480";
      minExtent = "8 8";
      visible = "1";
      setFirstResponder = "0";
      modal = "1";
      helpTag = "0";
      open = false;
   };
}


function Editor::onAdd(%this)
{
   // Basic stuff
   exec("./cursors.cs");

   // Tools
   exec("./editor.bind.cs");
   exec("./ObjectBuilderGui.gui");

   // New World Editor
   exec("./EditorGui.gui");
   exec("./EditorGui.cs");

   // World Editor
   exec("./WorldEditorSettingsDlg.gui");

   // Terrain Editor
   exec("./TerrainEditorVSettingsGui.gui");

   // Ignore Replicated fxStatic Instances.
   EWorldEditor.ignoreObjClass("fxShapeReplicatedStatic");

   // do gui initialization...
   EditorGui.init();

   //
   exec("./editorRender.cs");
}

function Editor::checkActiveLoadDone()
{
   if(isObject(EditorGui) && EditorGui.loadingMission)
   {
      Canvas.setContent(EditorGui);
      EditorGui.loadingMission = false;
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
function toggleEditor(%make)
{
   if (%make)
   {
      if (!$missionRunning) 
      {
         // just in case ...
         disconnect();
         Editor.close();
         
         // Allow users to override if they want.
         %newMission = "~/data/newMission.mis";
         if($Editor::newMissionOverride !$= "")
            %newMission = $Editor::newMissionOverride;
         
         createServer( "SinglePlayer", expandFilename(%newMission));
         %conn = new GameConnection(ServerConnection);
         RootGroup.add(ServerConnection);
         %conn.setConnectArgs($pref::Player::Name);
         %conn.setJoinPassword($Client::Password);
         %conn.connectLocal();
         
         Editor::create();
         MissionCleanup.add(Editor);
         EditorGui.loadingMission = true;
         EditorGui.saveAs = true;
         Editor.open();

         $dropcameracount = 0;
         schedule(100,0,dropFreakinCameraAtPlayer);
      }
      else
      {
         if (!isObject(Editor))
         {
            Editor::create();
            MissionCleanup.add(Editor);
         }
         
         if (Canvas.getContent() == EditorGui.getId())
            if (MissionInfo.type $= "DemoScene") 
            {
               commandToServer('dropPlayerAtCamera');
               Editor.close("SceneGui");   
            } 
            else 
            {
               Editor.close("PlayGui");
            }
         else 
         {
            if (MissionInfo.type $= "DemoScene")
               commandToServer('dropCameraAtPlayer');
               
            Editor.open();
         }
      }
   }
}

//------------------------------------------------------------------------------
//  The editor action maps are defined in editor.bind.cs
GlobalActionMap.bind(keyboard, "f11", toggleEditor);
