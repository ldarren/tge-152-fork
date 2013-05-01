//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-----------------------------------------------------------------------------


// Load up common script base
loadDir("common");

//-----------------------------------------------------------------------------
// Load up defaults console values.

// Defaults console values
exec("./client/defaults.cs");
exec("./server/defaults.cs");

// Preferences (overide defaults)
exec("./client/prefs.cs");
exec("./server/prefs.cs");


function displayHelp() {
   Parent::displayHelp();
   error(
      "FPS Mod options:\n"@
      "  -dedicated             Start as dedicated server\n"@
      "  -connect <address>     For non-dedicated: Connect to a game at <address>\n" @
      "  -mission <filename>    For dedicated: Load the mission\n"
      );
}

function parseArgs()
{
   // Arguments, which override everything else.
   for (%i = 1; %i < $Game::argc ; %i++)
   {
      %arg = $Game::argv[%i];
      %nextArg = $Game::argv[%i+1];
      %hasNextArg = $Game::argc - %i > 1;

      switch$ (%arg)
      {
         //--------------------
         case "-dedicated":
            $Server::Dedicated = true;
            enableWinConsole(true);
            $argUsed[%i]++;

            //--------------------
         case "-mission":
            $argUsed[%i]++;
            if (%hasNextArg) {
               $missionArg = %nextArg;
               $argUsed[%i+1]++;
               %i++;
            }
            else
               error("Error: Missing Command Line argument. Usage: -mission <filename>");

            //--------------------
         case "-connect":
            $argUsed[%i]++;
            if (%hasNextArg) {
               $JoinGameAddress = %nextArg;
               $argUsed[%i+1]++;
               %i++;
            }
            else
               error("Error: Missing Command Line argument. Usage: -connect <ip_address>");

            //--------------------
         case "-fullscreen":
            $pref::Video::fullScreen = 1;
            $argUsed[%i]++;

            //--------------------
         case "-windowed":
            $pref::Video::fullScreen = 0;
            $argUsed[%i]++;

            //--------------------
         case "-noSound":
            error("no support yet");
            $argUsed[%i]++;

            //--------------------
         case "-openGL":
            $pref::Video::displayDevice = "OpenGL";
            $argUsed[%i]++;

            //--------------------
         case "-directX":
            $pref::Video::displayDevice = "D3D";
            $argUsed[%i]++;

            //--------------------
         case "-voodoo2":
            $pref::Video::displayDevice = "Voodoo2";
            $argUsed[%i]++;

            //--------------------
         case "-autoVideo":
            $pref::Video::displayDevice = "";
            $argUsed[%i]++;

            //--------------------
         case "-prefs":
            $argUsed[%i]++;
            if (%hasNextArg) {
               exec(%nextArg, true, true);
               $argUsed[%i+1]++;
               %i++;
            }
            else
               error("Error: Missing Command Line argument. Usage: -prefs <path/script.cs>");
      }
   }
}

function onStart()
{
   echo("\c3\n--------- onStart(): GPGT Kit ---------");

  
   setRandomSeed();

   exec("./client/init.cs");

   exec("./server/init.cs");

   exec("./data/init.cs");

   initBaseClient();

   initBaseServer();

   initServer();

   if ($Server::Dedicated)
   {
      initDedicated();
   }
   else
   {
      initClient();
   }
}

function onExit()
{
   echo("\n--------- onExit(): GPGT Kit ---------");
   echo("Exporting client prefs");
   export("$pref::*", "./client/prefs.cs", False);

   echo("Exporting server prefs");
   export("$Pref::Server::*", "./server/prefs.cs", False);

   OpenALShutdown();
   Parent::onExit();
}
