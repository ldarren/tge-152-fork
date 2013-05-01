//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
function initBaseClient()
{
   echo("\c3\n--------- initBaseClient(): GPGT Kit ---------");
   setRandomSeed();

   //exec("./base/message.cs");
   //exec("./base/mission.cs");
   //exec("./base/missionDownload.cs");
   exec("./base/actionMap.cs");


   exec("./base/ui/defaultProfiles.cs");
   exec("./base/audio.cs");
   exec("./base/audioProfiles.cs");
   exec("./base/canvas.cs");
   exec("./base/default.bind.cs");
   exec("./base/message.cs");

   // Game information used to query the master server
   $Client::GameTypeQuery = "GPGT Lesson Kit";
   $Client::MissionTypeQuery = "Any";
}

function initClient()
{
   echo("\c3\n--------- initClient(): GPGT Kit ---------");

   //GPGT*********************************************************************
   //GPGT*********************************************************************
   //GPGT**  Auto Load GPGT Systems
   //GPGT*********************************************************************
   //GPGT*********************************************************************
   // In order to allow for distribution of this kit w/o .cs files,
   // The auto-loader code does the following:
   // 1. Attempt to load DSO files ONLY.
   // 2. Failing that, try the same for CS files.
   //
   // Note: One drawback of doing this is the fact that newly added CS files
   // will not be seen until you clean away all DSO files.  Of course,
   // this also keeps folks from 'extending' the free version of this kit, so
   // the tradeoff is probably fair.
   //
   %searchPattern = "*/egs_*.cs.dso";
   %sampleScriptFile = findFirstFile(%searchPattern);

   if("" $= %sampleScriptFile) 
   {
      %searchPattern     = "*/egs_*.cs";
      %sampleScriptFile = findFirstFile(%searchPattern);
   }

   while("" !$= %sampleScriptFile) 
   {
      %sampleScriptFile = strReplace( %sampleScriptFile , ".dso",  "" );

      exec( %sampleScriptFile );

      //echo("\c2 Autoloading: ", %sampleScriptFile);

      %sampleScriptFile = findNextFile(%searchPattern);
   }
   echo("");

   exec("./custom/loader.cs"); // Over-ride prior settings here

   initCanvas("GPGT - Lesson Kit", true); //GPGT

   exec("./scripts/clientCmds.cs");
   exec("./scripts/clientSideMissionDownload.cs");
   exec("./scripts/game.cs");
   exec("./scripts/mission.cs");
   exec("./scripts/missionLoaders.cs");
   exec("./scripts/serverConnection.cs");   

   exec("./sharedAssets/loader.cs");

   exec("./interfaces/mainMenu/loader.cs");
   
      
   exec("./interfaces/SampleScriptConsole/loader.cs");  

   //exec("./interfaces/options/loader.cs");

   //exec("./interfaces/missionLoader/loader.cs");

   exec("./interfaces/gameInterfaces/loader.cs");

   exec("./interfaces/missionLoading/loader.cs");

   exec("./interfaces/serverQuery/loader.cs");
   
   exec("./interfaces/splash/splash.cs");
   exec("./interfaces/splash2/splash.cs");

   //GPGT*********************************************************************
   //GPGT*********************************************************************
   //GPGT** Load GPGT GUI Samples
   //GPGT*********************************************************************
   //GPGT*********************************************************************
   exec("./interfaces/GUISamplerManager/loader.cs");

   //GPGT*********************************************************************
   //GPGT*********************************************************************
   //GPGT** Load GPGT Interface Samples
   //GPGT*********************************************************************
   //GPGT*********************************************************************
   exec("./interfaces/InterfaceSamplerManager/loader.cs");

   //GPGT*********************************************************************
   //GPGT*********************************************************************
   //GPGT**  Load Sample Scripts (Either for Guide or User Defined)
   //GPGT*********************************************************************
   //GPGT*********************************************************************
   echo("\n\c3--------- Loading Sample Scripts  ---------"); 
   // In order to allow for distribution of this kit w/o .cs files,
   // The auto-loader code does the following:
   // 1. Attempt to load DSO files ONLY.
   // 2. Failing that, try the same for CS files.
   //
   // Note: One drawback of doing this is the fact that newly added CS files
   // will not be seen until you clean away all DSO files.  Of course,
   // this also keeps folks from 'extending' the free version of this kit, so
   // the tradeoff is probably fair.
   //
   %searchPattern = "*/sts*.cs.dso";
   %sampleScriptFile = findFirstFile(%searchPattern);

   if("" $= %sampleScriptFile) 
   {
      %searchPattern     = "*/sts*.cs";
      %sampleScriptFile = findFirstFile(%searchPattern);
   }

   while("" !$= %sampleScriptFile) 
   {
      %sampleScriptFile = strReplace( %sampleScriptFile , ".dso",  "" );

      exec( %sampleScriptFile );
      %sampleScriptFile = findNextFile(%searchPattern);
   }

   //GPGT*********************************************************************
   //GPGT*********************************************************************
   //GPGT** Load GPGT Lesson Selector GUI and supporting scripts
   //GPGT*********************************************************************
   //GPGT*********************************************************************
    exec("./interfaces/GPGTLessonSelector/lessonSelector.cs");
    exec("./interfaces/GPGTLessonSelector/LessonMessage.gui");


    //initEditors(); 

   initRenderFeatures();

   if ($JoinGameAddress !$= "") 
   {
      loadMainMenu();
      connect($JoinGameAddress, "", $Pref::Player::Name);
   }
   else 
   {
      Canvas.setCursor("DefaultCursor");
      if ( $SkipSplashScreens ) 
      {
         Canvas.setContent( MainMenuGui );
      }
      else
      {
         Canvas.setContent( SplashScreen );
      }
   }
}

function initRenderFeatures()
{
   setShadowDetailLevel( $pref::shadows );
   setDefaultFov( $pref::Player::defaultFov );
   setZoomSpeed( $pref::Player::zoomSpeed );
}


// GPGT - Override the creator::editor::onAdd() so that we can select our own
//         location for the heightfields
package TerraformerOverride
{
   function Editor::onAdd(%this) 
   {
      Parent::onAdd(%this);
      $TerraformerHeightfieldDir = expandFilename("~/data/heightFields");
   }
};
activatePackage(TerraformerOverride);

