//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
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


//-----------------------------------------------------------------------------
// Package overrides to initialize the mod.
package demo {

function displayHelp() {
   Parent::displayHelp();
   error(
      "Demo options:\n"@
      "  -dedicated             Start as dedicated server\n"@
      "  -connect <address>     For non-dedicated: Connect to a game at <address>\n" @
      "  -mission <filename>    For dedicated: Load the mission\n"
   );
}

function parseArgs()
{
   Parent::parseArgs();

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
      }
   }
}

function onStart()
{
   Parent::onStart();
   echo("\n--------- Initializing MOD: Torque Demo ---------");

   // Load the scripts that start it all...
   exec("./client/init.cs");
   exec("./server/init.cs");

   // Server gets loaded for all sessions, since clients
   // can host in-game servers.
   initServer();

   // Start up in either client, or dedicated server mode
   if ($Server::Dedicated)
      initDedicated();
   else
      initClient();
      
   //------------------------------------------------------------------
   // TGE Modernization Kit
   //------------------------------------------------------------------
      
   exec("./data/init.cs");
   DRL::setBiasLimits(-0.05, 0);
   DRL::setGoalIntensity(0.35);
   DRL::setBloomColorOffset(0.15);
      
   //------------------------------------------------------------------
   // TGE Modernization Kit
   //------------------------------------------------------------------
}

function onExit()
{
   echo("Exporting client prefs");
   export("$pref::*", "./client/prefs.cs", False);

   echo("Exporting client config");
   if (isObject(moveMap))
      moveMap.save("./client/config.cs", false);

   echo("Exporting server prefs");
   export("$Pref::Server::*", "./server/prefs.cs", False);
   BanList::Export("./server/banlist.cs");

   Parent::onExit();
}

}; // Client package
activatePackage(demo);

$testscript::log = "";
$testscript::scenecount = 0;
$testscript::grabcount = 0;
$testscript::grabcounttotal = 0;
$testscript::fpsavg = 0;
$testscript::scenefps = 0;
$testscript::numscenes = 22;
$testscript::numgrabs = 50;

function testScript(%note) {
	$testscript::log = new FileObject();
	$testscript::log.openForAppend("demo/framecount.txt");
	$testscript::log.writeLine("=== " @ %note @ " ===");

	loadFeatureMission();
	schedule(10000,0,testThread);
}

function testThread() {
	// grab & average.
	%fps = $fps::real;
	%fps = (( $testscript::fpsavg * $testscript::grabcounttotal) + %fps ) / ( $testscript::grabcounttotal + 1);
	$testscript::fpsavg = %fps;
   
   %sceneFps = $fps::real;
   %sceneFps = (( $testscript::scenefps * $testscript::grabcount) + %sceneFps ) / ( $testscript::grabcount + 1);
   $testscript::scenefps = %sceneFps;
   
	$testscript::grabcounttotal++;
	$testscript::grabcount++;
	// if grabcount > numgrabs, next scene & reset
	if( $testscript::grabcount >= $testscript::numgrabs ) {
      $testscript::log.writeLine(" " @ SceneGui.getSceneName() @ " : " @ $testscript::sceneFps @ " fps");
		SceneGui.nextScene();
		$testscript::grabcount = 0;
      $testscript::sceneFps  = 0;
		$testscript::scenecount++;
	}
	// inc scenecount, if over numscenes, write file, close file, quit
	if( $testscript::scenecount > $testscript::numscenes ) {
		$testscript::log.writeLine(" avg fps:" SPC $testscript::fpsavg );
		$testscript::log.writeLine(" samples:" SPC $testscript::grabcounttotal );
		$testscript::log.close();
		quit();
	}
	// schedule self.
	schedule(500,0,testThread);
}
